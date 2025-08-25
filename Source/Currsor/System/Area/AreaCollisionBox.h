// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AreaCollisionBox.generated.h"

UCLASS()
class CURRSOR_API AAreaCollisionBox : public AActor
{
	GENERATED_BODY()

public:
	AAreaCollisionBox(const FObjectInitializer& ObjectInitializer);
	AAreaCollisionBox(const FObjectInitializer& ObjectInitializer, const FVector& InitialLocation);
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
	FString Name;

	UFUNCTION(BlueprintCallable, Category = "Area")
	void SetAreaID(const int32& InAreaID) { AreaID = InAreaID; }

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere)
	int32 AreaID;
};
