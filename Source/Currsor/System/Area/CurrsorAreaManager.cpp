// Fill out your copyright notice in the Description page of Project Settings.

#include "CurrsorAreaManager.h"
#include "AreaCollisionBox.h"
#include "Currsor/System/CurrsorGameState.h"

ACurrsorAreaManager::ACurrsorAreaManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACurrsorAreaManager::BeginPlay()
{
	Super::BeginPlay();

	if (ACurrsorGameState* State = Cast<ACurrsorGameState>(GetWorld()->GetGameState()))
	{
		State->SetAreaManager(this);
	}
}

void ACurrsorAreaManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TObjectPtr<AAreaCollisionBox> ACurrsorAreaManager::GetAreaBox(int32 ID)
{
	return BoxIDMap[ID];
}

void ACurrsorAreaManager::CreateAreaData()
{
	TObjectPtr<AAreaCollisionBox> NewCollisionBox = GetWorld()->SpawnActor<AAreaCollisionBox>(AAreaCollisionBox::StaticClass(), GetActorLocation(), GetActorRotation());

	check(NewCollisionBox);

	// Generate new ID
	int32 NewID = GetTypeHash(NewCollisionBox);
	
	NewCollisionBox->SetAreaID(NewID);
	NewCollisionBox->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	// Add to map
	if (!BoxIDMap.Contains(NewID))
	{
		BoxIDMap.Add(NewID, NewCollisionBox);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Duplicate Box ID generated: %d"), NewID);
	}
}

void ACurrsorAreaManager::RemoveAreaData()
{

}