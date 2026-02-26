// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrsorGameState.h"
#include "./Area/CurrsorAreaManager.h"
#include "./Area/AreaCollisionBox.h"
#include "CurrsorGameMode.h"

ACurrsorGameState::ACurrsorGameState()
{
	// 初始化战斗状态
	CombatState = ECombatState::Default;
	
	// 初始化玩家统计
	TotalPlayerCount = 0;
	PlayerDeathCount = 0;
	CurrentPlayerHealth = 100.0f;
	
	// 初始化敌人统计
	TotalEnemyCount = 0;
	EnemyDeathCount = 0;
	CurrentEnemyHealth = 100.0f;
	
	// 初始化位置
	LastPlayerCombatPosition = FVector::ZeroVector;
	LastEnemyCombatPosition = FVector::ZeroVector;
	
	// 初始化区域ID
	CurrentAreaID = 0;
}

TObjectPtr<AAreaCollisionBox> ACurrsorGameState::GetActorFromID(int32 InID) const
{
	return AreaManager->GetAreaBox(InID);
}

FString ACurrsorGameState::GetNameFromID(int32 InID) const
{
	TObjectPtr<AAreaCollisionBox> Actor = GetActorFromID(InID);
	return Actor ? Actor->Name : "Error";
}

void ACurrsorGameState::SetAreaManager(ACurrsorAreaManager* InAreaManager)
{
	check(InAreaManager);
	
	AreaManager = InAreaManager;
}

void ACurrsorGameState::SetCombatState(ECombatState InCombatState)
{
	if (CombatState == InCombatState) return;
	CombatState = InCombatState;

	switch (InCombatState)
	{
	case ECombatState::Combat:
		BroadcastCombatStateChange(TEXT("Combat"));
		break;
	case ECombatState::Victory:
		BroadcastCombatStateChange(TEXT("Victory"));
		break;
	case ECombatState::Defeat:
		BroadcastCombatStateChange(TEXT("Defeat"));
		break;
	default:
		break;
	}
}

void ACurrsorGameState::BroadcastCombatStateChange(const FString& CombatEventType)
{
	// 获取游戏模式并广播战斗状态变化
	if (ACurrsorGameMode* GameMode = Cast<ACurrsorGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->OnCombatStateChanged.Broadcast(CombatEventType);
		UE_LOG(LogTemp, Log, TEXT("广播战斗状态变化: %s"), *CombatEventType);
	}
}

void ACurrsorGameState::IncrementEnemyDeathCount()
{
	EnemyDeathCount++;
	UE_LOG(LogTemp, Log, TEXT("敌人死亡计数增加，当前死亡数: %d/%d"), EnemyDeathCount, TotalEnemyCount);
	
	// 检查是否所有敌人都已死亡
	if (AreAllEnemiesDead())
	{
		SetCombatState(ECombatState::Victory);
		UE_LOG(LogTemp, Log, TEXT("所有敌人已死亡，战斗胜利！"));
		
		// 延迟一段时间后退出战斗状态，确保战斗胜利动画和退出逻辑有足够时间完成
		FTimerHandle ExitCombatTimer;
		GetWorld()->GetTimerManager().SetTimer(ExitCombatTimer, [this]()
		{
			SetCombatState(ECombatState::Default);
			UE_LOG(LogTemp, Log, TEXT("战斗胜利后已退出战斗状态，恢复为默认状态"));
		}, 3.0f, false); // 3秒后退出战斗状态
	}
}

bool ACurrsorGameState::AreAllEnemiesDead() const
{
	return TotalEnemyCount > 0 && EnemyDeathCount >= TotalEnemyCount;
}

void ACurrsorGameState::IncrementPlayerDeathCount()
{
	PlayerDeathCount++;
	UE_LOG(LogTemp, Log, TEXT("玩家死亡计数增加，当前死亡数: %d/%d"), PlayerDeathCount, TotalPlayerCount);
	
	// 检查是否所有玩家都已死亡
	if (AreAllPlayersDead())
	{
		SetCombatState(ECombatState::Defeat);
		UE_LOG(LogTemp, Log, TEXT("所有玩家已死亡，战斗失败！"));
		// 战斗失败处理预留位置
	}
}

bool ACurrsorGameState::AreAllPlayersDead() const
{
	return TotalPlayerCount > 0 && PlayerDeathCount >= TotalPlayerCount;
}