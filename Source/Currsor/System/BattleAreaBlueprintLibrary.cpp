// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleAreaBlueprintLibrary.h"
#include "Engine/World.h"
#include "Components/BillboardComponent.h"
#include "Currsor/Character/Enemy/BaseEnemy.h"
#include "Currsor/Character/Player/CurrsorCharacter.h"
#include "Currsor/System/Area/AreaCollisionBox.h"
#include "Currsor/System/BattleAreaManager.h"
#include "Currsor/System/CurrsorGameState.h"
#include "Currsor/System/Area/CurrsorAreaManager.h"

UBattleAreaManager* UBattleAreaBlueprintLibrary::GetBattleAreaManager(const UObject* WorldContext)
{
	if (!WorldContext)
	{
		return nullptr;
	}

	UWorld* World = WorldContext->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	return World->GetGameInstance()->GetSubsystem<UBattleAreaManager>();
}

void UBattleAreaBlueprintLibrary::SetEnemyAreaID(ABaseEnemy* Enemy, int32 AreaID)
{
	if (!Enemy)
	{
		return;
	}

	UBattleAreaManager* Manager = GetBattleAreaManager(Enemy);
	if (Manager)
	{
		Manager->SetEnemyAreaID(Enemy, AreaID);
	}
	else
	{
		// 如果管理器不可用，直接设置到敌人身上
		Enemy->SetAreaID(AreaID);
	}
}

int32 UBattleAreaBlueprintLibrary::GetEnemyAreaID(ABaseEnemy* Enemy)
{
	if (!Enemy)
	{
		return -1;
	}

	UBattleAreaManager* Manager = GetBattleAreaManager(Enemy);
	if (Manager)
	{
		return Manager->GetEnemyAreaID(Enemy);
	}

	return Enemy->GetAreaID();
}

void UBattleAreaBlueprintLibrary::AssignEnemiesInArea(AAreaCollisionBox* AreaBox, float SearchRadius)
{
	if (!AreaBox)
	{
		return;
	}

	UBattleAreaManager* Manager = GetBattleAreaManager(AreaBox);
	if (Manager)
	{
		Manager->AssignEnemiesInArea(AreaBox, SearchRadius);
	}
}

bool UBattleAreaBlueprintLibrary::DoesEnemyHaveAreaID(ABaseEnemy* Enemy)
{
	if (!Enemy)
	{
		return false;
	}

	UBattleAreaManager* Manager = GetBattleAreaManager(Enemy);
	if (Manager)
	{
		return Manager->DoesEnemyHaveAreaID(Enemy);
	}

	return Enemy->HasAreaID();
}

TArray<ABaseEnemy*> UBattleAreaBlueprintLibrary::GetEnemiesInArea(const UObject* WorldContext, int32 AreaID)
{
	UBattleAreaManager* Manager = GetBattleAreaManager(WorldContext);
	if (Manager)
	{
		return Manager->GetEnemiesInArea(AreaID);
	}

	return TArray<ABaseEnemy*>();
}

void UBattleAreaBlueprintLibrary::ClearEnemyAreaID(ABaseEnemy* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	UBattleAreaManager* Manager = GetBattleAreaManager(Enemy);
	if (Manager)
	{
		Manager->ClearEnemyAreaID(Enemy);
	}
	else
	{
		Enemy->SetAreaID(-1);
	}
}

void UBattleAreaBlueprintLibrary::SetEnemySelectedAreaBox(ABaseEnemy* Enemy, AAreaCollisionBox* AreaBox)
{
	if (!Enemy)
	{
		return;
	}

	Enemy->SetSelectedAreaBox(AreaBox);
}

AAreaCollisionBox* UBattleAreaBlueprintLibrary::GetEnemySelectedAreaBox(ABaseEnemy* Enemy)
{
	if (!Enemy)
	{
		return nullptr;
	}

	return Enemy->GetSelectedAreaBox();
}

void UBattleAreaBlueprintLibrary::ReadEnemyAreaIDFromSelectedBox(ABaseEnemy* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	Enemy->ReadAreaIDFromSelectedBox();
}

bool UBattleAreaBlueprintLibrary::DoesEnemyHaveSelectedAreaBox(ABaseEnemy* Enemy)
{
	if (!Enemy)
	{
		return false;
	}

	return Enemy->HasSelectedAreaBox();
}

bool UBattleAreaBlueprintLibrary::GetAreaPositions(AAreaCollisionBox* AreaBox, FVector& PlayerPosition, FVector& EnemyPosition, FVector& CameraPosition)
{
	if (!AreaBox)
	{
		return false;
	}

	// 获取所有Billboard组件
	TArray<UBillboardComponent*> BillboardComponents;
	AreaBox->GetComponents<UBillboardComponent>(BillboardComponents);

	UBillboardComponent* PlayerBillboard = nullptr;
	UBillboardComponent* EnemyBillboard = nullptr;
	UBillboardComponent* CameraBillboard = nullptr;

	// 通过名称查找特定的Billboard组件
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
		UE_LOG(LogTemp, Warning, TEXT("获取区域位置: 区域中缺少billboard components"));
		return false;
	}

	PlayerPosition = PlayerBillboard->GetComponentLocation();
	EnemyPosition = EnemyBillboard->GetComponentLocation();
	CameraPosition = CameraBillboard->GetComponentLocation();

	return true;
}

bool UBattleAreaBlueprintLibrary::ValidateBattleTeleportConditions(ACurrsorCharacter* Player, ABaseEnemy* Enemy)
{
	if (!Player || !Enemy)
	{
		return false;
	}

	if (Enemy->IsDead())
	{
		return false;
	}

	// 现在检查玩家或敌人是否有有效的区域ID
	if (!Player->HasValidAreaID() && !Enemy->HasAreaID())
	{
		return false;
	}

	return true;
}

void UBattleAreaBlueprintLibrary::SetPlayerAreaID(ACurrsorCharacter* Player, int32 AreaID)
{
	if (!Player)
	{
		return;
	}

	Player->SetCurrentAreaID(AreaID);
}

int32 UBattleAreaBlueprintLibrary::GetPlayerAreaID(ACurrsorCharacter* Player)
{
	if (!Player)
	{
		return -1;
	}

	return Player->GetCurrentAreaID();
}

bool UBattleAreaBlueprintLibrary::DoesPlayerHaveValidAreaID(ACurrsorCharacter* Player)
{
	if (!Player)
	{
		return false;
	}

	return Player->HasValidAreaID();
}

void UBattleAreaBlueprintLibrary::ClearPlayerAreaID(ACurrsorCharacter* Player)
{
	if (!Player)
	{
		return;
	}

	Player->SetCurrentAreaID(0);
}

bool UBattleAreaBlueprintLibrary::GetEffectiveAreaID(ACurrsorCharacter* Player, ABaseEnemy* Enemy, int32& OutAreaID, bool& OutIsFromPlayer)
{
	if (!Player || !Enemy)
	{
		OutAreaID = -1;
		OutIsFromPlayer = false;
		return false;
	}

	// 优先使用玩家的区域ID
	if (Player->HasValidAreaID())
	{
		OutAreaID = Player->GetCurrentAreaID();
		OutIsFromPlayer = true;
		return true;
	}

	// 如果玩家没有有效的区域ID，使用敌人的区域ID
	if (Enemy->HasAreaID())
	{
		OutAreaID = Enemy->GetAreaID();
		OutIsFromPlayer = false;
		return true;
	}

	// 两者都没有有效的区域ID
	OutAreaID = -1;
	OutIsFromPlayer = false;
	return false;
}

// ========== 测试假人管理实现 ==========
void UBattleAreaBlueprintLibrary::SpawnTestDummiesForArea(AAreaCollisionBox* AreaBox)
{
	if (!AreaBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("战斗区域蓝图库: 区域碰撞盒为空"));
		return;
	}

	AreaBox->SpawnTestDummies();
}

void UBattleAreaBlueprintLibrary::DestroyTestDummiesForArea(AAreaCollisionBox* AreaBox)
{
	if (!AreaBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("战斗区域蓝图库: 区域碰撞盒为空"));
		return;
	}

	AreaBox->DestroyTestDummies();
}

void UBattleAreaBlueprintLibrary::SpawnAllTestDummies(const UObject* WorldContext)
{
	if (!WorldContext)
	{
		UE_LOG(LogTemp, Warning, TEXT("战斗区域蓝图库: 世界上下文为空"));
		return;
	}

	UWorld* World = WorldContext->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("战斗区域蓝图库: 世界对象为空"));
		return;
	}

	// 通过GameState获取AreaManager
	if (ACurrsorGameState* GameState = World->GetGameState<ACurrsorGameState>())
	{
		if (ACurrsorAreaManager* AreaManager = GameState->GetAreaManager())
		{
			AreaManager->SpawnAllTestDummies();
		}
		else
		{
		UE_LOG(LogTemp, Warning, TEXT("BattleAreaBlueprintLibrary: 未找到 AreaManager"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleAreaBlueprintLibrary: 未找到 GameState"));
	}
}

void UBattleAreaBlueprintLibrary::DestroyAllTestDummies(const UObject* WorldContext)
{
	if (!WorldContext)
	{
		UE_LOG(LogTemp, Warning, TEXT("战斗区域蓝图库: 世界上下文为空"));
		return;
	}

	UWorld* World = WorldContext->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("战斗区域蓝图库: 世界对象为空"));
		return;
	}

	// 通过GameState获取AreaManager
	if (ACurrsorGameState* GameState = World->GetGameState<ACurrsorGameState>())
	{
		if (ACurrsorAreaManager* AreaManager = GameState->GetAreaManager())
		{
			AreaManager->DestroyAllTestDummies();
		}
		else
		{
		UE_LOG(LogTemp, Warning, TEXT("BattleAreaBlueprintLibrary: 未找到 AreaManager"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleAreaBlueprintLibrary: 未找到 GameState"));
	}
}