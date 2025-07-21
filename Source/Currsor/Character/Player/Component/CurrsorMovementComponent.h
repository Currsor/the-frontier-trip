// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CurrsorMovementComponent.generated.h"


class ACurrsorPlayerController;
class ACurrsorPlayerState;
class ACurrsorCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CURRSOR_API UCurrsorMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCurrsorMovementComponent();

	void Initialize(ACurrsorCharacter* InPlayer, ACurrsorPlayerState* InState, ACurrsorPlayerController* InController);

	UFUNCTION()
	void Move(const FInputActionValue& Value);
	void UpdateRotationBasedOnInput(float DeltaTime);

	UFUNCTION()
	void SetMovementSpeed(float NewSpeed);
	
	UFUNCTION()
	float GetMovementSpeed() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<ACurrsorCharacter> CurrsorPlayer;
	
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<ACurrsorPlayerState> CurrsorPlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<ACurrsorPlayerController> CurrsorPlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Config|Dash", meta = (AllowPrivateAccess = "true"))
	float CurrentMovementVector = 0.0f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Config|Movement", meta = (AllowPrivateAccess = "true"))
	float MovementSpeed = 600.0f;
};
