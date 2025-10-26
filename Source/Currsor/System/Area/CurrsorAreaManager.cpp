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
	
	NewCollisionBox->SetOwner(this);
	
	NewCollisionBox->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	// Add to map
	if (!BoxIDMap.Contains(NewID))
	{
		BoxIDMap.Add(NewID, NewCollisionBox);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("生成了重复的盒子ID: %d"), NewID);
	}
}

void ACurrsorAreaManager::RemoveAreaData()
{
	for (auto It = BoxIDMap.CreateIterator(); It; ++It)
	{
		if (It->Value == nullptr)
		{
			It.RemoveCurrent();
		}
	}
}

void ACurrsorAreaManager::SpawnAllTestDummies()
{
	UE_LOG(LogTemp, Log, TEXT("AreaManager: 为所有区域生成测试假人"));
	
	for (auto& Pair : BoxIDMap)
	{
		if (Pair.Value && IsValid(Pair.Value))
		{
			Pair.Value->SpawnTestDummies();
		}
	}
}

void ACurrsorAreaManager::DestroyAllTestDummies()
{
	UE_LOG(LogTemp, Log, TEXT("AreaManager: 销毁所有区域的测试假人"));
	
	for (auto& Pair : BoxIDMap)
	{
		if (Pair.Value && IsValid(Pair.Value))
		{
			Pair.Value->DestroyTestDummies();
		}
	}
}