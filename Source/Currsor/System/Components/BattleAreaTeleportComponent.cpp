// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleAreaTeleportComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BillboardComponent.h"
#include "Currsor/Character/Player/CurrsorCharacter.h"
#include "Currsor/Character/Enemy/BaseEnemy.h"
#include "Currsor/System/Area/AreaCollisionBox.h"
#include "Currsor/System/CurrsorGameState.h"

UBattleAreaTeleportComponent::UBattleAreaTeleportComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBattleAreaTeleportComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (bEnableDebugLogging)
	{
		UE_LOG(LogTemp, Log, TEXT("BattleAreaTeleportComponent initialized"));
	}
}

bool UBattleAreaTeleportComponent::ProcessBattleAreaTeleport(ACurrsorCharacter* Player, ABaseEnemy* Enemy)
{
	if (!ValidateTeleportConditions(Player, Enemy))
	{
		return false;
	}

	// 优先使用玩家的AreaID，如果无效则使用敌人的AreaID
	int32 AreaID = 0;
	bool bUsingPlayerAreaID = false;
	
	// 首先检查玩家是否有有效的区域ID（通过GameState）
	if (Player->HasValidAreaID())
	{
		AreaID = Player->GetCurrentAreaID();
		bUsingPlayerAreaID = true;
		
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Log, TEXT("Using Player's Area ID (from GameState): %d"), AreaID);
		}
	}
	// 如果玩家没有有效的区域ID，检查敌人的区域ID
	else if (Enemy->HasAreaID())
	{
		AreaID = Enemy->GetAreaID();
		bUsingPlayerAreaID = false;
		
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Log, TEXT("Player has no valid Area ID, using Enemy's Area ID: %d"), AreaID);
		}
	}
	// 如果两者都没有有效的区域ID，跳过传送
	else
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Warning, TEXT("Neither Player nor Enemy %s has a valid area ID, skipping teleport"), *Enemy->GetName());
		}
		return false;
	}
	AAreaCollisionBox* AreaBox = GetAreaCollisionBox(AreaID);
	
	if (!AreaBox)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not find area collision box for ID: %d"), AreaID);
		}
		return false;
	}

	// 获取Billboard位置和旋转
	UBillboardComponent* PlayerBillboard = AreaBox->FindComponentByClass<UBillboardComponent>();
	UBillboardComponent* EnemyBillboard = nullptr;
	UBillboardComponent* CameraBillboard = nullptr;

	// 通过名称查找特定的Billboard组件
	TArray<UBillboardComponent*> BillboardComponents;
	AreaBox->GetComponents<UBillboardComponent>(BillboardComponents);
	
	for (UBillboardComponent* Billboard : BillboardComponents)
	{
		FString ComponentName = Billboard->GetName();
		if (ComponentName.Contains(TEXT("Player")))
		{
			PlayerBillboard = Billboard;
		}
		else if (ComponentName.Contains(TEXT("Enemy")))
		{
			EnemyBillboard = Billboard;
		}
		else if (ComponentName.Contains(TEXT("Camera")))
		{
			CameraBillboard = Billboard;
		}
	}

	if (!PlayerBillboard || !EnemyBillboard || !CameraBillboard)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Error, TEXT("Missing billboard components in area %d"), AreaID);
		}
		return false;
	}

	// 执行传送
	FVector PlayerTargetLocation = PlayerBillboard->GetComponentLocation();
	FRotator PlayerTargetRotation = PlayerBillboard->GetComponentRotation();
	
	FVector EnemyTargetLocation = EnemyBillboard->GetComponentLocation();
	FRotator EnemyTargetRotation = EnemyBillboard->GetComponentRotation();
	
	FVector CameraTargetLocation = CameraBillboard->GetComponentLocation();
	FRotator CameraTargetRotation = CameraBillboard->GetComponentRotation();

	// 延迟执行传送以确保攻击动画完成
	FTimerHandle TeleportTimer;
	GetWorld()->GetTimerManager().SetTimer(TeleportTimer, [this, Player, Enemy, PlayerTargetLocation, PlayerTargetRotation, EnemyTargetLocation, EnemyTargetRotation, CameraTargetLocation, CameraTargetRotation, AreaID, bUsingPlayerAreaID]()
	{
		TeleportPlayer(Player, PlayerTargetLocation, PlayerTargetRotation);
		TeleportEnemy(Enemy, EnemyTargetLocation, EnemyTargetRotation);
		MoveCameraToPosition(Player, CameraTargetLocation, CameraTargetRotation);
		
		if (bEnableDebugLogging)
		{
			FString AreaIDSource = bUsingPlayerAreaID ? TEXT("Player") : TEXT("Enemy");
			UE_LOG(LogTemp, Log, TEXT("Battle area teleport completed for area ID: %d (from %s)"), AreaID, *AreaIDSource);
		}
	}, TeleportDelay, false);

	return true;
}

void UBattleAreaTeleportComponent::TeleportPlayer(ACurrsorCharacter* Player, const FVector& TargetLocation, const FRotator& TargetRotation)
{
	if (!Player)
	{
		return;
	}

	Player->SetActorLocation(TargetLocation);
	Player->SetActorRotation(TargetRotation);

	if (bEnableDebugLogging)
	{
		UE_LOG(LogTemp, Log, TEXT("Player teleported to: %s"), *TargetLocation.ToString());
	}
}

void UBattleAreaTeleportComponent::TeleportEnemy(ABaseEnemy* Enemy, const FVector& TargetLocation, const FRotator& TargetRotation)
{
	if (!Enemy)
	{
		return;
	}

	Enemy->SetActorLocation(TargetLocation);
	Enemy->SetActorRotation(TargetRotation);

	if (bEnableDebugLogging)
	{
		UE_LOG(LogTemp, Log, TEXT("Enemy %s teleported to: %s"), *Enemy->GetName(), *TargetLocation.ToString());
	}
}

void UBattleAreaTeleportComponent::MoveCameraToPosition(ACurrsorCharacter* Player, const FVector& TargetLocation, const FRotator& TargetRotation)
{
	if (!Player)
	{
		return;
	}

	// 获取玩家的弹簧臂组件
	USpringArmComponent* SpringArm = Player->FindComponentByClass<USpringArmComponent>();
	if (!SpringArm)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player has no SpringArmComponent, cannot move camera"));
		}
		return;
	}

	// 设置弹簧臂的位置和旋转
	SpringArm->SetWorldLocation(TargetLocation);
	SpringArm->SetWorldRotation(TargetRotation);

	if (bEnableDebugLogging)
	{
		UE_LOG(LogTemp, Log, TEXT("Camera moved to: %s"), *TargetLocation.ToString());
	}
}

AAreaCollisionBox* UBattleAreaTeleportComponent::GetAreaCollisionBox(int32 AreaID) const
{
	ACurrsorGameState* GameState = GetCurrsorGameState();
	if (!GameState)
	{
		return nullptr;
	}

	return GameState->GetActorFromID(AreaID).Get();
}

bool UBattleAreaTeleportComponent::ValidateTeleportConditions(ACurrsorCharacter* Player, ABaseEnemy* Enemy) const
{
	if (!Player)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Error, TEXT("Player is null"));
		}
		return false;
	}

	if (!Enemy)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Error, TEXT("Enemy is null"));
		}
		return false;
	}

	if (Enemy->IsDead())
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy %s is dead, skipping teleport"), *Enemy->GetName());
		}
		return false;
	}

	return true;
}

ACurrsorGameState* UBattleAreaTeleportComponent::GetCurrsorGameState() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	return World->GetGameState<ACurrsorGameState>();
}