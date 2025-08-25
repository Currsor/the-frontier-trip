// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrsorGameState.h"
#include "./Area/CurrsorAreaManager.h"
#include "./Area/AreaCollisionBox.h"

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
