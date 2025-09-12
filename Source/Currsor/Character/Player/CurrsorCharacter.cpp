// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrsorCharacter.h"

#include "Component/CurrsorCameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Currsor/Component/HealthComponent.h"
#include "Currsor/System/GameSystemManager.h"
#include "Currsor/System/Components/AttackSystemComponent.h"

ACurrsorCharacter::ACurrsorCharacter()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	SpringArmComponent -> SetupAttachment( RootComponent);
	
	CameraComponent = CreateDefaultSubobject<UCurrsorCameraComponent>(TEXT("Camera"));
	CameraComponent -> SetupAttachment(SpringArmComponent);
	
	AttackHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Attack Hitbox"));
	AttackHitbox->SetupAttachment(RootComponent);
	AttackHitbox->SetCollisionProfileName(TEXT("NoCollision"));
	
	// 设置攻击碰撞盒的大小和位置
	AttackHitbox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	AttackHitbox->SetRelativeLocation(FVector(60.0f, 0.0f, 0.0f)); // 在角色前方
	
	UE_LOG(LogTemp, Warning, TEXT("CurrsorCharacter AttackHitbox created and configured"));

	// 创建生命值组件
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	HealthComponent->SetMaxHealth(100.0f);
	
	const FVector Start = SpringArmComponent->GetComponentLocation();
	const FVector End = SpringArmComponent->GetSocketLocation(SpringArmComponent->SocketName);
	const float CurrentLength = FVector::Distance(Start, End);
	
	LastArmLength = CurrentLength;
	LastCollisionState = SpringArmComponent->IsCollisionFixApplied();
	
	CameraComponent->UpdateDOF(CurrentLength, LastCollisionState, SpringArmComponent->TargetArmLength);
	
	PrimaryActorTick.bCanEverTick = true;
}

void ACurrsorCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 初始化游戏系统管理器
	GameSystemManager = UGameSystemManager::GetInstance(GetWorld());
	if (GameSystemManager && !GameSystemManager->IsInitialized())
	{
		GameSystemManager->Initialize(GetWorld());
	}

	// 获取攻击系统引用
	if (GameSystemManager)
	{
		AttackSystem = GameSystemManager->GetAttackSystem();
	}
}

void ACurrsorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 更新DOF
	if (SpringArmComponent && CameraComponent)
	{
		bool CurrentCollisionState = SpringArmComponent->IsCollisionFixApplied();
		
		const FVector Start = SpringArmComponent->GetComponentLocation();
		const FVector End = SpringArmComponent->GetSocketLocation(SpringArmComponent->SocketName);
		
		const float CurrentLength = FVector::Distance(Start, End);
		
		CameraComponent->UpdateDOF(CurrentLength, CurrentCollisionState, SpringArmComponent->TargetArmLength);
		
		LastArmLength = CurrentLength;
		LastCollisionState = CurrentCollisionState;
	}
}

void ACurrsorCharacter::SetHitboxCollision(bool bCollision)
{
	AttackHitbox->SetCollisionProfileName(bCollision ? TEXT("OverlapAll") : TEXT("NoCollision"));
}

void ACurrsorCharacter::ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator, const FHitResult& HitResult)
{
	IDamageable::ApplyDamage_Implementation(DamageAmount, DamageInstigator, HitResult);

	if (!HealthComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("HealthComponent is null!"));
		return;
	}

	// 应用伤害
	HealthComponent->TakeDamage(DamageAmount);
	
	if (HealthComponent->IsDead())
	{
		// 死亡逻辑
		UE_LOG(LogTemp, Warning, TEXT("Player Die"));
		CurrsorPlayerState->ChangeState(ECharacterState::Dead);
	}
	else
	{
		// 受击逻辑
		UE_LOG(LogTemp, Warning, TEXT("Player Take Damage: %f, Current Health: %f"), 
			DamageAmount, HealthComponent->GetCurrentHealth());
		CurrsorPlayerState->ChangeState(ECharacterState::Hurt);
	}
	
}

float ACurrsorCharacter::GetHealth() const
{
	return HealthComponent ? HealthComponent->GetCurrentHealth() : 0.0f;
}

float ACurrsorCharacter::GetMaxHealth() const
{
	return HealthComponent ? HealthComponent->GetMaxHealth() : 0.0f;
}

bool ACurrsorCharacter::IsDead() const
{
	return HealthComponent ? HealthComponent->IsDead() : false;
}
