// Fill out your copyright notice in the Description page of Project Settings.

#include "AreaCollisionBox.h"

#include "Components/BillboardComponent.h"
#include "../CurrsorGameState.h"
#include "Components/ArrowComponent.h"
#include "Currsor/Character/Player/CurrsorCharacter.h"
#include "Currsor/System/CurrsorGameInstance.h"

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
		FVector SymmetricLocation = FVector(MainLocation.X * 2 - PlayerLocation.X, MainLocation.Y * 2 - PlayerLocation.Y, PlayerLocation.Z);
		EnemyBillboard->SetWorldLocation(SymmetricLocation);
	}
	else if (UpdatedComponent == EnemyBillboard)
	{
		FVector MainLocation = MainBattleBillboard->GetComponentLocation();
		FVector EnemyLocation = EnemyBillboard->GetComponentLocation();
		FVector SymmetricLocation = FVector(MainLocation.X * 2 - EnemyLocation.X, MainLocation.Y * 2 - EnemyLocation.Y, EnemyLocation.Z);
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
	
	PlayerBillboard->SetRelativeLocation(FVector(0.0f, -200.0f, 0.0f));
	EnemyBillboard->SetRelativeLocation(FVector(0.0f, 200.0f, 0.0f));
	CameraBillboard->SetRelativeLocation(FVector(200.0f, 0.0f, 0.0f));
	
	// 设置PlayerBillboard和EnemyBillboard的旋转角度，使其x轴指向MainBattleBillboard
	PlayerBillboard->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	EnemyBillboard->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	CameraBillboard->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	
	// 添加箭头组件，旋转角度与父类一致
	PlayerArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("PlayerArrow"));
	PlayerArrow->SetupAttachment(PlayerBillboard);

	EnemyArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("EnemyArrow"));
	EnemyArrow->SetupAttachment(EnemyBillboard);

	CameraArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("CameraArrow"));
	CameraArrow->SetupAttachment(CameraBillboard);
	
	// 添加事件绑定
	PlayerBillboard->TransformUpdated.AddUObject(this, &AAreaCollisionBox::UpdateSymmetricBillboard);
	EnemyBillboard->TransformUpdated.AddUObject(this, &AAreaCollisionBox::UpdateSymmetricBillboard);
}