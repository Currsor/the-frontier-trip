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

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	TObjectPtr<AAreaCollisionBox> GetAreaBox(int32 ID);
	
	UFUNCTION(CallInEditor, Category = "Area Management")
	void CreateAreaData();

	UFUNCTION(CallInEditor, Category = "Area Management")
	void RemoveAreaData();

private:
	// 编号与Box的映射表
	UPROPERTY(VisibleAnywhere)
	TMap<int32, TObjectPtr<AAreaCollisionBox>> BoxIDMap;
};