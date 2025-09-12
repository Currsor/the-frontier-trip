// Fill out your copyright notice in the Description page of Project Settings.

#include "AreaCollisionBox.h"

#include "../CurrsorGameState.h"
#include "Currsor/Character/Player/CurrsorCharacter.h"
#include "Currsor/System/CurrsorGameInstance.h"

// 设置纹理路径常量
const FSoftObjectPath MAIN_TEXTURE_PATH(TEXT("/Engine/EditorResources/S_ReflActorIcon.S_ReflActorIcon"));
const FSoftObjectPath PLAYER_TEXTURE_PATH(TEXT("/Engine/EngineResources/AICON-Green.AICON-Green"));
const FSoftObjectPath ENEMY_TEXTURE_PATH(TEXT("/Engine/EngineResources/AICON-Red.AICON-Red"));
const FSoftObjectPath CAMERA_TEXTURE_PATH(TEXT("/Engine/Tutorial/Basics/TutorialAssets/CameraActor_64x.CameraActor_64x"));

// Sets default values
AAreaCollisionBox::AAreaCollisionBox(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), MainTexture(MAIN_TEXTURE_PATH), PlayerTexture(PLAYER_TEXTURE_PATH), EnemyTexture(ENEMY_TEXTURE_PATH), CameraTexture(CAMERA_TEXTURE_PATH)
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxCollision;
	
	BoxCollision->SetBoxExtent(FVector(100.f));
	BoxCollision->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AAreaCollisionBox::OnOverlapBegin);

	SetupBillboards();
}

AAreaCollisionBox::AAreaCollisionBox(const FObjectInitializer& ObjectInitializer, const FVector& InitialLocation) : Super(ObjectInitializer), MainTexture(MAIN_TEXTURE_PATH), PlayerTexture(PLAYER_TEXTURE_PATH), EnemyTexture(ENEMY_TEXTURE_PATH), CameraTexture(CAMERA_TEXTURE_PATH)
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
	MainBattleBillboard->bIsScreenSizeScaled = true;
	if (MainTexture.IsValid()) MainBattleBillboard->SetSprite(MainTexture.Get());

	PlayerBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("PlayerBillboard"));
	PlayerBillboard->SetupAttachment(MainBattleBillboard);
	if (PlayerTexture.IsValid()) PlayerBillboard->SetSprite(PlayerTexture.Get());

	EnemyBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("EnemyBillboard"));
	EnemyBillboard->SetupAttachment(MainBattleBillboard);
	if (EnemyTexture.IsValid()) EnemyBillboard->SetSprite(EnemyTexture.Get());

	CameraBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("CameraBillboard"));
	CameraBillboard->SetupAttachment(MainBattleBillboard);
	if (CameraTexture.IsValid()) CameraBillboard->SetSprite(CameraTexture.Get());
	
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