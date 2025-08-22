// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrsorGameState.h"
#include "./Area/CurrsorAreaManager.h"
#include "./Area/AreaCollisionBox.h"

AActor* ACurrsorGameState::GetActorFromID(int32 InID) const
{
	return AreaManager->GetAreaBox(InID);
}
