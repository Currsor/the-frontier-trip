// Fill out your copyright notice in the Description page of Project Settings.

#include "AreaCollisionBox.h"
#include "../CurrsorGameState.h"
#include "Currsor/Character/Player/CurrsorCharacter.h"

// Sets default values
AAreaCollisionBox::AAreaCollisionBox(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxCollision;
	
	BoxCollision->SetBoxExtent(FVector(100.f));
	BoxCollision->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AAreaCollisionBox::OnOverlapBegin);
}

AAreaCollisionBox::AAreaCollisionBox(const FObjectInitializer& ObjectInitializer, const FVector& InitialLocation) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxCollision;
	
	BoxCollision->SetWorldLocation(InitialLocation);
	BoxCollision->SetBoxExtent(FVector(100.f));
	
	BoxCollision->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AAreaCollisionBox::OnOverlapBegin);
}

void AAreaCollisionBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin: %s"), *OtherActor->GetName());
	
	if (OtherActor && !OtherActor->IsA<ACurrsorCharacter>()) return;
	if (OtherComp && OtherComp->GetAttachParent() != nullptr) return;
	
	if (ACurrsorGameState* GameState = GetWorld()->GetGameState<ACurrsorGameState>())
	{
		GameState->SetCurrentAreaID(AreaID);
	}
}

