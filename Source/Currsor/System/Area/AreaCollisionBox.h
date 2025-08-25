// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AreaCollisionBox.generated.h"

class ABattleBillboard;

UCLASS()
class CURRSOR_API AAreaCollisionBox : public AActor
{
	GENERATED_BODY()

public:
	AAreaCollisionBox(const FObjectInitializer& ObjectInitializer);
	AAreaCollisionBox(const FObjectInitializer& ObjectInitializer, const FVector& InitialLocation);
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currsor|Area")
	FString Name;

	UFUNCTION(BlueprintCallable, Category = "Currsor|Area")
	void SetAreaID(const int32& InAreaID) { AreaID = InAreaID; }

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void UpdateSymmetricBillboard(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);

private:
	UPROPERTY(VisibleAnywhere)
	int32 AreaID;

	// ========== 战斗位置 ==========
	
	UPROPERTY(VisibleAnywhere)
	UBillboardComponent* MainBattleBillboard;

	UPROPERTY(VisibleAnywhere)
	UBillboardComponent* PlayerBillboard;

	UPROPERTY(VisibleAnywhere)
	UBillboardComponent* EnemyBillboard;

	UPROPERTY(VisibleAnywhere)
	UBillboardComponent* CameraBillboard;

	UPROPERTY(VisibleAnywhere, Category = "Currsor|Billboard")
	UTexture2D* MainTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Engine/MobileResources/HUD/AnalogHat.AnalogHat"));

	UPROPERTY(VisibleAnywhere, Category = "Currsor|Billboard")
	UTexture2D* PlayerTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Engine/EngineResources/AICON-Green.AICON-Green"));

	UPROPERTY(VisibleAnywhere, Category = "Currsor|Billboard")
	UTexture2D* EnemyTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Engine/EngineResources/AICON-Red.AICON-Red"));

	UPROPERTY(VisibleAnywhere, Category = "Currsor|Billboard")
	UTexture2D* CameraTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Engine/Tutorial/Basics/TutorialAssets/CameraActor_64x.CameraActor_64x"));

	void SetupBillboards();
};