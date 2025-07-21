// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrsorMovementComponent.h"

#include "InputActionValue.h"
#include "Currsor/Character/Player/CurrsorCharacter.h"
#include "Currsor/Character/Player/CurrsorPlayerController.h"
#include "Currsor/Character/Player/CurrsorPlayerState.h"


// Sets default values for this component's properties
UCurrsorMovementComponent::UCurrsorMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCurrsorMovementComponent::Initialize(ACurrsorCharacter* InPlayer, ACurrsorPlayerState* InState,ACurrsorPlayerController* InController)
{
	CurrsorPlayer = InPlayer;
	CurrsorPlayerState = InState;
	CurrsorPlayerController = InController;
}

void UCurrsorMovementComponent::Move(const FInputActionValue& Value)
{
	// TODO: 这里可以添加更多的输入处理逻辑
	// if (!CurrsorPlayerState->CanMove()) return;
	
	if (CurrsorPlayer && CurrsorPlayerController)
	{
		// 获取输入值（Axis2D 类型）
		const FVector2D MovementVector = Value.Get<FVector2D>();
	
		// 获取角色前方和右方方向
		const FRotator Rotation = CurrsorPlayerController->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 计算移动方向
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 应用移动
		CurrsorPlayer->AddMovementInput(ForwardDirection, MovementVector.X);
		CurrsorPlayer->AddMovementInput(RightDirection, MovementVector.Y);

		// 仅当左右移动时更新当前移动向量
		if (FMath::Abs(MovementVector.X) > KINDA_SMALL_NUMBER)
		{
			CurrentMovementVector = MovementVector.X;
		}
	}
}

void UCurrsorMovementComponent::UpdateRotationBasedOnInput(float DeltaTime)
{
	if (this&& FMath::Abs(CurrentMovementVector) > KINDA_SMALL_NUMBER)
	{
		// 计算移动方向
		const FVector RightDirection = FVector::XAxisVector;

		// 根据当前移动向量设置目标旋转
		const FRotator TargetRotation = FRotationMatrix::MakeFromX((CurrentMovementVector > 0) ? RightDirection : -RightDirection).Rotator();
		CurrsorPlayer->SetActorRotation(FMath::RInterpTo(CurrsorPlayer->GetActorRotation(), TargetRotation, DeltaTime, 10.0f));
	}
}

void UCurrsorMovementComponent::SetMovementSpeed(float NewSpeed)
{
	MovementSpeed = FMath::Clamp(NewSpeed, 0.0f, 1200.0f);
	if (CurrsorPlayer)
	{
		CurrsorPlayer->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrsorPlayer is null in UCurrsorMovementComponent::SetMovementSpeed"));
	}
}

float UCurrsorMovementComponent::GetMovementSpeed() const
{
	return MovementSpeed;
}
