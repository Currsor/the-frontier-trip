// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrsorCharacter.h"

#include "Component/CurrsorCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACurrsorCharacter::ACurrsorCharacter()
{
	//弹簧臂组件
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	SpringArmComponent -> SetupAttachment( RootComponent);
    
	//相机组件
	CameraComponent = CreateDefaultSubobject<UCurrsorCameraComponent>(TEXT("Camera"));
	CameraComponent -> SetupAttachment(SpringArmComponent);

	// 初始化弹簧臂长度和碰撞状态
	const FVector Start = SpringArmComponent->GetComponentLocation();
	const FVector End = SpringArmComponent->GetSocketLocation(SpringArmComponent->SocketName);
	const float CurrentLength = FVector::Distance(Start, End);
	
	// 初始化LastArmLength和LastCollisionState
	LastArmLength = CurrentLength;
	LastCollisionState = SpringArmComponent->IsCollisionFixApplied();
	
	// 使用新的UpdateDOF函数
	CameraComponent->UpdateDOF(CurrentLength, LastCollisionState, SpringArmComponent->TargetArmLength);
	
	// 启用tick
	PrimaryActorTick.bCanEverTick = true;
}

void ACurrsorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 检查组件是否有效
	if (SpringArmComponent && CameraComponent)
	{
		// 获取当前碰撞状态
		bool CurrentCollisionState = SpringArmComponent->IsCollisionFixApplied();
		
		// 获取弹簧臂的起点和终点位置
		const FVector Start = SpringArmComponent->GetComponentLocation();
		const FVector End = SpringArmComponent->GetSocketLocation(SpringArmComponent->SocketName);
		
		// 计算实际长度（考虑碰撞）
		const float CurrentLength = FVector::Distance(Start, End);
		
		// 使用新的UpdateDOF函数，传入当前长度、碰撞状态和目标臂长
		CameraComponent->UpdateDOF(CurrentLength, CurrentCollisionState, SpringArmComponent->TargetArmLength);
		
		// 更新上一次的值
		LastArmLength = CurrentLength;
		LastCollisionState = CurrentCollisionState;
	}
}