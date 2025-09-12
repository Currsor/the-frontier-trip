// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Currsor/Character/Component/BaseState.h"
#include "Currsor/Component/HealthComponent.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	// 创建生命值组件
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	if (HealthComponent)
	{
		HealthComponent->SetMaxHealth(100.0f);
	}

	// 创建状态组件
	CurrentState = CreateDefaultSubobject<ABaseState>(TEXT("CurrentState"));
	if (CurrentState)
	{
		CurrentState->ChangeState(ECharacterState::Idle);
	}
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// 绑定生命值组件的死亡事件
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ABaseEnemy::OnHealthDepleted);
	}
}

void ABaseEnemy::ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator, const FHitResult& HitResult)
{
	IDamageable::ApplyDamage_Implementation(DamageAmount, DamageInstigator, HitResult);

	// 如果已经死亡或没有生命值组件，不处理伤害
	if (IsDead() || !HealthComponent) return;

	// 应用伤害到生命值组件
	HealthComponent->TakeDamage(DamageAmount, DamageInstigator);

	// 如果没有死亡，切换到受伤状态
	if (!IsDead() && CurrentState)
	{
		CurrentState->ChangeState(ECharacterState::Hurt);
		
		// 调用蓝图事件
		OnTakeDamageBP(DamageAmount, DamageInstigator);
	}

	UE_LOG(LogTemp, Log, TEXT("%s took %f damage from %s. Health: %f/%f"), 
		   *GetName(), 
		   DamageAmount, 
		   DamageInstigator ? *DamageInstigator->GetName() : TEXT("Unknown"),
		   HealthComponent->GetCurrentHealth(),
		   HealthComponent->GetMaxHealth());
}

bool ABaseEnemy::IsDead() const
{
	return HealthComponent ? HealthComponent->IsDead() : false;
}

void ABaseEnemy::OnHealthDepleted(AActor* DeadActor)
{
	HandleDeath();
}

void ABaseEnemy::HandleDeath()
{
	if (IsDead())
	{
		// 切换到死亡状态
		if (CurrentState)
		{
			CurrentState->ChangeState(ECharacterState::Dead);
		}

		// 禁用碰撞
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 广播死亡事件
		OnEnemyDeath.Broadcast(this);
		OnDeathBP();

		UE_LOG(LogTemp, Warning, TEXT("%s has died"), *GetName());
	}
}