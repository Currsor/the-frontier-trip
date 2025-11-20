// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrsorCharacter.h"

#include "Component/CurrsorCameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Currsor/Component/HealthComponent.h"
#include "Currsor/System/GameSystemManager.h"
#include "Currsor/System/Components/AttackSystemComponent.h"
#include "Currsor/System/Components/BattleAreaTeleportComponent.h"
#include "Currsor/System/CurrsorGameState.h"
#include "CurrsorPlayerController.h"
#include "CurrsorPlayerState.h"
#include "Component/CurrsorActionComponent.h"

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

	// 创建生命值组件
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	HealthComponent->SetMaxHealth(100.0f);

	// 创建战斗区域传送组件
	BattleAreaTeleportComponent = CreateDefaultSubobject<UBattleAreaTeleportComponent>(TEXT("Battle Area Teleport Component"));
	
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
		
		// 设置攻击系统的战斗区域传送组件
		if (AttackSystem && BattleAreaTeleportComponent)
		{
			AttackSystem->SetBattleAreaTeleportComponent(BattleAreaTeleportComponent);
		}
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
		UE_LOG(LogTemp, Warning, TEXT("HealthComponent 为空!"));
		return;
	}

	// 应用伤害
	HealthComponent->TakeDamage(DamageAmount);
	
	// 获取PlayerState
	ACurrsorPlayerState* CurrsorPlayerState = GetPlayerState<ACurrsorPlayerState>();
	if (!CurrsorPlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("无法获取PlayerState!"));
		return;
	}
	
	if (HealthComponent->IsDead())
	{
		// 死亡逻辑
		UE_LOG(LogTemp, Warning, TEXT("玩家死亡"));
		CurrsorPlayerState->ChangeState(ECharacterState::Dead);
	}
	else
	{
		// 受击逻辑
		UE_LOG(LogTemp, Warning, TEXT("玩家受到伤害: %f, 当前生命值: %f"), 
			DamageAmount, HealthComponent->GetCurrentHealth());
		CurrsorPlayerState->ChangeState(ECharacterState::Hurt);
	}
	
}

APlayerState* ACurrsorCharacter::GetCurrsorPlayerState() const
{
	return GetPlayerState<ACurrsorPlayerState>();
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

// ========== 区域ID相关方法实现 ==========

int32 ACurrsorCharacter::GetCurrentAreaID() const
{
	if (const ACurrsorGameState* GameState = GetWorld()->GetGameState<ACurrsorGameState>())
	{
		return GameState->GetCurrentAreaID();
	}
	return 0;
}

void ACurrsorCharacter::SetCurrentAreaID(int32 NewAreaID)
{
	if (ACurrsorGameState* GameState = GetWorld()->GetGameState<ACurrsorGameState>())
	{
		GameState->SetCurrentAreaID(NewAreaID);
	}
}

bool ACurrsorCharacter::HasValidAreaID() const
{
	return GetCurrentAreaID() > 0;
}

void ACurrsorCharacter::SetRotationAdjustmentEnabled(bool bEnabled)
{
	if (ACurrsorPlayerController* PlayerController = Cast<ACurrsorPlayerController>(GetController()))
	{
		if (UCurrsorActionComponent* ActionComponent = PlayerController->GetPlayerActionComponent())
		{
			ActionComponent->SetRotationAdjustmentEnabled(bEnabled);
		}
	}
}

bool ACurrsorCharacter::IsRotationAdjustmentEnabled() const
{
	if (ACurrsorPlayerController* PlayerController = Cast<ACurrsorPlayerController>(GetController()))
	{
		if (UCurrsorActionComponent* ActionComponent = PlayerController->GetPlayerActionComponent())
		{
			return ActionComponent->IsRotationAdjustmentEnabled();
		}
	}
	return false;
}

void ACurrsorCharacter::ResetRotationAdjustment()
{
	if (ACurrsorPlayerController* PlayerController = Cast<ACurrsorPlayerController>(GetController()))
	{
		if (UCurrsorActionComponent* ActionComponent = PlayerController->GetPlayerActionComponent())
		{
			ActionComponent->ResetRotationAdjustment();
		}
	}
}

void ACurrsorCharacter::SwitchToPlayerCamera()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		// 切换回玩家角色作为视角目标
		PlayerController->SetViewTarget(this);
		UE_LOG(LogTemp, Log, TEXT("已切换回玩家摄像机"));
	}
}

void ACurrsorCharacter::SwitchToBattleCamera(UBillboardComponent* CameraBillboard)
{
	if (BattleAreaTeleportComponent)
	{
		BattleAreaTeleportComponent->SwitchToBattleCamera(this, CameraBillboard);
	}
}
