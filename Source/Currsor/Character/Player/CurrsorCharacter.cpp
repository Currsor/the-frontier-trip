// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrsorCharacter.h"

#include "Component/CurrsorCameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACurrsorCharacter::ACurrsorCharacter()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	SpringArmComponent -> SetupAttachment( RootComponent);
	
	CameraComponent = CreateDefaultSubobject<UCurrsorCameraComponent>(TEXT("Camera"));
	CameraComponent -> SetupAttachment(SpringArmComponent);
	
	AttackHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Attack Hitbox"));
	AttackHitbox->SetupAttachment(RootComponent);
	
	const FVector Start = SpringArmComponent->GetComponentLocation();
	const FVector End = SpringArmComponent->GetSocketLocation(SpringArmComponent->SocketName);
	const float CurrentLength = FVector::Distance(Start, End);
	
	LastArmLength = CurrentLength;
	LastCollisionState = SpringArmComponent->IsCollisionFixApplied();
	
	CameraComponent->UpdateDOF(CurrentLength, LastCollisionState, SpringArmComponent->TargetArmLength);
	
	PrimaryActorTick.bCanEverTick = true;
}

void ACurrsorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 更新DOF
	if (SpringArmComponent && CameraComponent)
	{
		bool CurrentCollisionState = SpringArmComponent->IsCollisionFixApplied();
		
		const FVector Start = SpringArmComponent->GetComponentLocation();
		const FVector End = SpringArmComponent->GetSocketLocation(SpringArmComponent->SocketName);
		
		const float CurrentLength = FVector::Distance(Start, End);
		
		CameraComponent->UpdateDOF(CurrentLength, CurrentCollisionState, SpringArmComponent->TargetArmLength);
		
		LastArmLength = CurrentLength;
		LastCollisionState = CurrentCollisionState;
	}
}