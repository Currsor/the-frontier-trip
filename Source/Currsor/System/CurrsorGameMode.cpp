// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrsorGameMode.h"

#include "EngineUtils.h"
#include "Currsor/Character/Player/CurrsorCharacter.h"
#include "Currsor/Character/Player/CurrsorPlayerController.h"
#include "Currsor/System/Components/BattleAreaTeleportComponent.h"

void ACurrsorGameMode::StartPlay()
{
	Super::StartPlay();

	for (APlayerController* PC : TActorRange<APlayerController>(GetWorld()))
	{
		PC->SetShowMouseCursor(true); // 显示所有玩家的鼠标
	}

	OnCombatStateChanged.AddDynamic(this, &ACurrsorGameMode::HandleCombatStarted);
}

void ACurrsorGameMode::HandleCombatStarted(const FString& CombatEventType)
{
	if (CombatEventType=="Encounter")
	{
	UE_LOG(LogTemp, Warning, TEXT("遇敵"));
	}
	else if (CombatEventType=="Ambushed")
	{
	UE_LOG(LogTemp, Warning, TEXT("伏击"));
	}
	else if (CombatEventType=="Victory")
	{
	UE_LOG(LogTemp, Warning, TEXT("战斗胜利"));
	// 战斗胜利处理逻辑
	
		// 查找玩家角色并退出战斗区域
		for (TActorIterator<ACurrsorCharacter> It(GetWorld()); It; ++It)
		{
			ACurrsorCharacter* Player = *It;
			if (Player && Player->IsPlayerControlled())
			{
				// 获取战斗区域传送组件并退出战斗
				if (UBattleAreaTeleportComponent* BattleComponent = Player->FindComponentByClass<UBattleAreaTeleportComponent>())
				{
					BattleComponent->ExitBattleArea(Player);
					UE_LOG(LogTemp, Log, TEXT("战斗胜利：正在退出战斗区域并恢复玩家镜头"));
				}
				
				// 关闭战斗传送Widget
				if (ACurrsorPlayerController* PlayerController = Cast<ACurrsorPlayerController>(Player->GetController()))
				{
					PlayerController->CloseBattleTeleportWidget();
					UE_LOG(LogTemp, Log, TEXT("战斗胜利：已关闭战斗传送Widget"));
				}
				
				break;
			}
		}
	}
	else if (CombatEventType=="Defeat")
	{
	UE_LOG(LogTemp, Warning, TEXT("战斗失败"));
	// 战斗失败处理逻辑
	
		// 查找玩家角色并关闭战斗传送Widget
		for (TActorIterator<ACurrsorCharacter> It(GetWorld()); It; ++It)
		{
			ACurrsorCharacter* Player = *It;
			if (Player && Player->IsPlayerControlled())
			{
				// 关闭战斗传送Widget
				if (ACurrsorPlayerController* PlayerController = Cast<ACurrsorPlayerController>(Player->GetController()))
				{
					PlayerController->CloseBattleTeleportWidget();
					UE_LOG(LogTemp, Log, TEXT("战斗失败：已关闭战斗传送Widget"));
				}
				
				break;
			}
		}
	}
}
