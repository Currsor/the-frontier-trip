// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BaseState.generated.h"

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Walk        UMETA(DisplayName = "Walk"),
	Run         UMETA(DisplayName = "Run"),
	Jump        UMETA(DisplayName = "Jump"),
	Fall        UMETA(DisplayName = "Fall"),
	Attack      UMETA(DisplayName = "Attack"),
	RunAttack   UMETA(DisplayName = "RunAttack"),
	Dash        UMETA(DisplayName = "Dash"),

	Hurt        UMETA(DisplayName = "Hurt"),
	Dead        UMETA(DisplayName = "Dead"),
};

/**
 * 
 */
UCLASS()
class CURRSOR_API ABaseState : public APlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Player State")
	bool IsDead() const;

	UFUNCTION(BlueprintPure, Category = "Player State")
	bool IsHurt() const;
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetHealth(float NewHealth) { Health = NewHealth; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetMaxHealth(float NewMaxHealth) { MaxHealth = NewMaxHealth; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHealth;
};