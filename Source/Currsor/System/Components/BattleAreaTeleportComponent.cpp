// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleAreaTeleportComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/Engine.h"
#include "Currsor/Character/Player/CurrsorCharacter.h"
#include "Currsor/Character/Player/CurrsorPlayerController.h"
#include "Currsor/Character/Player/Component/CurrsorActionComponent.h"
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
		UE_LOG(LogTemp, Log, TEXT("BattleAreaTeleportComponent 已初始化"));
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
			UE_LOG(LogTemp, Log, TEXT("使用玩家的区域ID（来自 GameState）: %d"), AreaID);
		}
	}
	// 如果玩家没有有效的区域ID，检查敌人的区域ID
	else if (Enemy->HasAreaID())
	{
		AreaID = Enemy->GetAreaID();
		bUsingPlayerAreaID = false;
		
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Log, TEXT("玩家没有有效的区域ID，使用敌人的区域ID: %d"), AreaID);
		}
	}
	// 如果两者都没有有效的区域ID，跳过传送
	else
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Warning, TEXT("玩家和敌人 %s 都没有有效的区域ID，跳过传送"), *Enemy->GetName());
		}
		return false;
	}
	AAreaCollisionBox* AreaBox = GetAreaCollisionBox(AreaID);
	
	if (!AreaBox)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Error, TEXT("无法找到ID为 %d 的区域碰撞盒"), AreaID);
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
			UE_LOG(LogTemp, Error, TEXT("区域 %d 中缺少 billboard components"), AreaID);
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

	// 保存玩家战斗前的原始位置和旋转
	SavedPlayerLocation = Player->GetActorLocation();
	SavedPlayerRotation = Player->GetActorRotation();
	bHasSavedPlayerTransform = true;

	// 保存敌人战斗前的原始位置和旋转
	SavedEnemyLocation = Enemy->GetActorLocation();
	SavedEnemyRotation = Enemy->GetActorRotation();
	bHasSavedEnemyTransform = true;

	// 缓存敌人引用，用于退出战斗时恢复位置
	CachedBattleEnemy = Enemy;

	if (bEnableDebugLogging)
	{
		UE_LOG(LogTemp, Log, TEXT("已保存玩家原始位置: %s，旋转: %s"), *SavedPlayerLocation.ToString(), *SavedPlayerRotation.ToString());
		UE_LOG(LogTemp, Log, TEXT("已保存敌人原始位置: %s，旋转: %s"), *SavedEnemyLocation.ToString(), *SavedEnemyRotation.ToString());
	}

	// 延迟执行传送以确保攻击动画完成
	FTimerHandle TeleportTimer;
	GetWorld()->GetTimerManager().SetTimer(TeleportTimer, [this, Player, Enemy, PlayerTargetLocation, PlayerTargetRotation, EnemyTargetLocation, EnemyTargetRotation, CameraBillboard, AreaID, bUsingPlayerAreaID]()
	{
		TeleportPlayer(Player, PlayerTargetLocation, PlayerTargetRotation);
		TeleportEnemy(Enemy, EnemyTargetLocation, EnemyTargetRotation);
		SwitchToBattleCamera(Player, CameraBillboard);
		
		// 战斗开始：从被攻击的敌人身上读取敌人总数并初始化游戏状态
		if (ACurrsorGameState* GameState = GetCurrsorGameState())
		{
			// 从敌人身上读取敌人总数
			int32 TotalEnemies = Enemy ? Enemy->GetTotalEnemyCount() : 1;
			
			// 初始化游戏状态的敌人统计
			GameState->SetTotalEnemyCount(TotalEnemies);
			GameState->SetEnemyDeathCount(0);
			
			// 统计玩家总数（通常为1）
			GameState->SetTotalPlayerCount(1);
			GameState->SetPlayerDeathCount(0);
			
			// 设置战斗状态
			GameState->SetCombatState(ECombatState::Combat);
			
			if (bEnableDebugLogging)
			{
				UE_LOG(LogTemp, Log, TEXT("战斗开始：敌人 %s 代表的敌人总数为 %d，玩家总数：1"), 
					   Enemy ? *Enemy->GetName() : TEXT("未知"), TotalEnemies);
			}
		}
		
		// 传送完成后切换到战斗输入模式
		if (ACurrsorPlayerController* PlayerController = Cast<ACurrsorPlayerController>(Player->GetController()))
		{
			PlayerController->SwitchInputMappingContext(true); // true = 切换到战斗输入模式
			
			if (bEnableDebugLogging)
			{
				UE_LOG(LogTemp, Log, TEXT("战斗传送完成后已切换到战斗输入模式"));
			}
		}
		
		if (bEnableDebugLogging)
		{
			FString AreaIDSource = bUsingPlayerAreaID ? TEXT("玩家") : TEXT("敌人");
			UE_LOG(LogTemp, Log, TEXT("战斗区域传送完成，区域ID: %d（来自 %s）"), AreaID, *AreaIDSource);
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

	// 获取玩家控制器并禁用旋转调整
	if (ACurrsorPlayerController* PlayerController = Cast<ACurrsorPlayerController>(Player->GetController()))
	{
		if (UCurrsorActionComponent* ActionComponent = PlayerController->GetPlayerActionComponent())
		{
			ActionComponent->SetRotationAdjustmentEnabled(false);
			
			if (bEnableDebugLogging)
			{
				UE_LOG(LogTemp, Log, TEXT("传送到战斗区域后已禁用玩家的旋转调整"));
			}
		}
	}

	if (bEnableDebugLogging)
	{
		UE_LOG(LogTemp, Log, TEXT("玩家传送到: %s"), *TargetLocation.ToString());
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
		UE_LOG(LogTemp, Log, TEXT("敌人 %s 传送到: %s"), *Enemy->GetName(), *TargetLocation.ToString());
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
			UE_LOG(LogTemp, Warning, TEXT("玩家没有 SpringArmComponent，无法移动摄像机"));
		}
		return;
	}

	// 设置弹簧臂的位置和旋转
	SpringArm->SetWorldLocation(TargetLocation);
	SpringArm->SetWorldRotation(TargetRotation);

	if (bEnableDebugLogging)
	{
		UE_LOG(LogTemp, Log, TEXT("摄像机移动到: %s"), *TargetLocation.ToString());
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
			UE_LOG(LogTemp, Error, TEXT("玩家为空"));
		}
		return false;
	}

	if (!Enemy)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Error, TEXT("敌人为空"));
		}
		return false;
	}

	if (Enemy->IsDead())
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Warning, TEXT("敌人 %s 已死亡，跳过传送"), *Enemy->GetName());
		}
		return false;
	}

	return true;
}

void UBattleAreaTeleportComponent::SwitchToBattleCamera(ACurrsorCharacter* Player, UBillboardComponent* CameraBillboard)
{
	if (!Player || !CameraBillboard)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Warning, TEXT("玩家或 CameraBillboard 为空，无法切换摄像机"));
		}
		return;
	}

	// 获取玩家控制器
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController());
	if (!PlayerController)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Warning, TEXT("玩家没有 PlayerController，无法切换摄像机"));
		}
		return;
	}

	// 获取CameraBillboard所在的AreaCollisionBox
	AAreaCollisionBox* AreaBox = Cast<AAreaCollisionBox>(CameraBillboard->GetOwner());
	if (!AreaBox)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Warning, TEXT("CameraBillboard 的所有者不是 AreaCollisionBox，无法找到战斗摄像机"));
		}
		return;
	}

	// 直接获取AreaCollisionBox中的BattleCameraComponent
	UCameraComponent* BattleCamera = AreaBox->FindComponentByClass<UCameraComponent>();
	if (!BattleCamera)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Warning, TEXT("在 AreaCollisionBox 中未找到 BattleCameraComponent，无法切换摄像机"));
		}
		return;
	}

	// 切换视角目标到战斗相机所在的Actor（即AreaCollisionBox）
	PlayerController->SetViewTarget(AreaBox);

	if (bEnableDebugLogging)
	{
		UE_LOG(LogTemp, Log, TEXT("已将摄像机切换到 AreaCollisionBox 中的战斗摄像机: %s"), *AreaBox->GetName());
	}
}

void UBattleAreaTeleportComponent::ExitBattleArea(ACurrsorCharacter* Player)
{
	if (!Player)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Warning, TEXT("玩家为空，无法退出战斗区域"));
		}
		return;
	}

	// 延迟执行退出战斗以确保战斗结束动画完成
	FTimerHandle ExitBattleTimer;
	GetWorld()->GetTimerManager().SetTimer(ExitBattleTimer, [this, Player]()
	{
		// 恢复玩家到战斗前的原始位置和旋转
		if (bHasSavedPlayerTransform)
		{
			Player->SetActorLocation(SavedPlayerLocation);
			Player->SetActorRotation(SavedPlayerRotation);
			bHasSavedPlayerTransform = false;

			if (bEnableDebugLogging)
			{
				UE_LOG(LogTemp, Log, TEXT("已恢复玩家到原始位置: %s，旋转: %s"), *SavedPlayerLocation.ToString(), *SavedPlayerRotation.ToString());
			}
		}

		// 恢复敌人到战斗前的原始位置和旋转
		if (bHasSavedEnemyTransform && CachedBattleEnemy.IsValid())
		{
			CachedBattleEnemy->SetActorLocation(SavedEnemyLocation);
			CachedBattleEnemy->SetActorRotation(SavedEnemyRotation);
			bHasSavedEnemyTransform = false;
			CachedBattleEnemy = nullptr;

			if (bEnableDebugLogging)
			{
				UE_LOG(LogTemp, Log, TEXT("已恢复敌人到原始位置: %s，旋转: %s"), *SavedEnemyLocation.ToString(), *SavedEnemyRotation.ToString());
			}
		}

		// 切换回玩家相机
		SwitchToPlayerCamera(Player);
		
		// 切换回普通输入模式
		if (ACurrsorPlayerController* PlayerController = Cast<ACurrsorPlayerController>(Player->GetController()))
		{
			PlayerController->SwitchInputMappingContext(false); // false = 切换到普通输入模式
			
			if (bEnableDebugLogging)
			{
				UE_LOG(LogTemp, Log, TEXT("退出战斗区域后已切换到普通输入模式"));
			}
		}
		
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Log, TEXT("已退出战斗区域"));
		}
	}, ExitBattleDelay, false);
}

void UBattleAreaTeleportComponent::SwitchToPlayerCamera(ACurrsorCharacter* Player)
{
	if (!Player)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Warning, TEXT("玩家为空，无法切换回玩家相机"));
		}
		return;
	}

	// 获取玩家控制器
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController());
	if (!PlayerController)
	{
		if (bEnableDebugLogging)
		{
			UE_LOG(LogTemp, Warning, TEXT("玩家没有 PlayerController，无法切换相机"));
		}
		return;
	}

	// 切换视角目标回玩家
	PlayerController->SetViewTarget(Player);

	// 重新启用玩家的旋转调整
	if (ACurrsorPlayerController* CurrsorPlayerController = Cast<ACurrsorPlayerController>(PlayerController))
	{
		if (UCurrsorActionComponent* ActionComponent = CurrsorPlayerController->GetPlayerActionComponent())
		{
			ActionComponent->SetRotationAdjustmentEnabled(true);
			
			if (bEnableDebugLogging)
			{
				UE_LOG(LogTemp, Log, TEXT("退出战斗区域后已重新启用玩家的旋转调整"));
			}
		}
	}

	if (bEnableDebugLogging)
	{
		UE_LOG(LogTemp, Log, TEXT("已切换回玩家相机"));
	}
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