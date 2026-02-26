// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Currsor/Character/Component/BaseState.h"
#include "Currsor/Component/HealthComponent.h"
#include "Currsor/System/Area/AreaCollisionBox.h"
#include "Currsor/System/CurrsorGameState.h"

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

	// 自动从选中的AreaCollisionBox读取AreaID
	ReadAreaIDFromSelectedBox();
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

	UE_LOG(LogTemp, Log, TEXT("%s 受到来自 %s 的 %f 点伤害。生命值: %f/%f"), 
		   *GetName(), 
		   DamageInstigator ? *DamageInstigator->GetName() : TEXT("未知"),
		   DamageAmount, 
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

		// 通知游戏状态增加敌人死亡计数
		if (ACurrsorGameState* GameState = Cast<ACurrsorGameState>(GetWorld()->GetGameState()))
		{
			GameState->IncrementEnemyDeathCount();
		}

		UE_LOG(LogTemp, Warning, TEXT("%s 已死亡"), *GetName());
	}
}

void ABaseEnemy::ReadAreaIDFromSelectedBox()
{
	if (SelectedAreaBox)
	{
		// 从选中的AreaCollisionBox读取AreaID
		int32 NewAreaID = SelectedAreaBox->GetAreaID();
		if (NewAreaID != -1)
		{
			AreaID = NewAreaID;
			UE_LOG(LogTemp, Log, TEXT("敌人 %s 自动从选定的 AreaCollisionBox %s 读取区域ID %d"), 
				   *GetName(), *SelectedAreaBox->GetName(), AreaID);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("敌人 %s: 选定的 AreaCollisionBox %s 具有无效的区域ID (-1)"), 
				   *GetName(), *SelectedAreaBox->GetName());
		}
	}
	else if (AreaID == -1)
	{
		UE_LOG(LogTemp, Log, TEXT("敌人 %s: 未选择 AreaCollisionBox 且未设置手动区域ID"), *GetName());
	}
}