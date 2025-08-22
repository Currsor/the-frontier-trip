// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CurrsorAreaManager.generated.h"


class AAreaCollisionBox;

UCLASS(BlueprintType, Blueprintable)
class CURRSOR_API ACurrsorAreaManager : public AActor
{
	GENERATED_BODY()

public:
	ACurrsorAreaManager();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	AAreaCollisionBox* GetAreaBox(int32 ID) const { return BoxIDMap.FindRef(ID); }
	
	UFUNCTION(CallInEditor, Category = "Area Management")
	void CreateAreaData();

	UFUNCTION(CallInEditor, Category = "Area Management")
	void RemoveAreaData();

private:
	// 编号与Box的映射表
	UPROPERTY(VisibleAnywhere)
	TMap<int32, AAreaCollisionBox*> BoxIDMap;
};