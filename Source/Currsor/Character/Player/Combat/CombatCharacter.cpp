// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatCharacter.h"

#include "Currsor/Character/Player/Component/CurrsorCameraComponent.h"

ACombatCharacter::ACombatCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCurrsorCameraComponent>(TEXT("Camera"));
	CameraComponent -> SetupAttachment(RootComponent);
}
