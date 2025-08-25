// Fill out your copyright notice in the Description page of Project Settings.

#include "AreaCollisionBox.h"

#include "Components/BillboardComponent.h"
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

	SetupBillboards();
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

	SetupBillboards();
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

void AAreaCollisionBox::UpdateSymmetricBillboard(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	if (UpdatedComponent == PlayerBillboard)
	{
		FVector MainLocation = MainBattleBillboard->GetComponentLocation();
		FVector PlayerLocation = PlayerBillboard->GetComponentLocation();
		FVector SymmetricLocation = MainLocation * 2 - PlayerLocation;
		EnemyBillboard->SetWorldLocation(SymmetricLocation);
	}
	else if (UpdatedComponent == EnemyBillboard)
	{
		FVector MainLocation = MainBattleBillboard->GetComponentLocation();
		FVector EnemyLocation = EnemyBillboard->GetComponentLocation();
		FVector SymmetricLocation = MainLocation * 2 - EnemyLocation;
		PlayerBillboard->SetWorldLocation(SymmetricLocation);
	}
}

void AAreaCollisionBox::SetupBillboards()
{
	MainBattleBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("MainBattleBillboard"));
	MainBattleBillboard->SetupAttachment(RootComponent);
	MainBattleBillboard->SetSprite(MainTexture);

	PlayerBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("PlayerBillboard"));
	PlayerBillboard->SetupAttachment(MainBattleBillboard);
	PlayerBillboard->SetSprite(PlayerTexture);

	EnemyBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("EnemyBillboard"));
	EnemyBillboard->SetupAttachment(MainBattleBillboard);
	EnemyBillboard->SetSprite(EnemyTexture);

	CameraBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("CameraBillboard"));
	CameraBillboard->SetupAttachment(MainBattleBillboard);
	CameraBillboard->SetSprite(CameraTexture);
	
	PlayerBillboard->SetRelativeLocation(FVector(0.0f, -100.0f, 0.0f));
	EnemyBillboard->SetRelativeLocation(FVector(0.0f, 100.0f, 0.0f));
	CameraBillboard->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
	
	// 添加事件绑定
	PlayerBillboard->TransformUpdated.AddUObject(this, &AAreaCollisionBox::UpdateSymmetricBillboard);
	EnemyBillboard->TransformUpdated.AddUObject(this, &AAreaCollisionBox::UpdateSymmetricBillboard);
}