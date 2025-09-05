// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrsorGameMode.h"

#include "EngineUtils.h"

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
		UE_LOG(LogTemp, Warning, TEXT("Encounter"));
	}
	else if (CombatEventType=="Ambushed")
	{
		UE_LOG(LogTemp, Warning, TEXT("Ambushed"));
	}
}
