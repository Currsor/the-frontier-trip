// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrsorCameraComponent.h"

UCurrsorCameraComponent::UCurrsorCameraComponent()
{
	this->PostProcessSettings.bOverride_DepthOfFieldFstop = true;
	this->PostProcessSettings.DepthOfFieldFstop = 0.4f;           // Aperture (F-stop)
	this->PostProcessSettings.bOverride_DepthOfFieldSensorWidth = true;
	this->PostProcessSettings.DepthOfFieldSensorWidth = 144.0f;   // Sensor Width (mm)
	this->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	this->PostProcessSettings.DepthOfFieldFocalDistance = 1000.0f; // Focal Distance (cm)

	this->MarkRenderStateDirty();                                 // 强制刷新渲染状态
	
	// 初始化碰撞状态
	bWasInCollision = false;
}

void UCurrsorCameraComponent::UpdateDOF(float FocalDistance, bool bIsCollision, float TargetArmLength)
{
	if (bIsCollision)
	{
		bWasInCollision = true;
		// 正常更新焦距
		this->PostProcessSettings.DepthOfFieldFocalDistance = FocalDistance;
		// 更新碰撞状态
		bWasInCollision = bIsCollision;
		// 强制刷新渲染状态
		this->MarkRenderStateDirty();
	}
	else if (bWasInCollision)
	{
		// 使用目标臂长作为焦距
		this->PostProcessSettings.DepthOfFieldFocalDistance = TargetArmLength;
		// 重置碰撞状态
		bWasInCollision = false;
		// 强制刷新渲染状态
		this->MarkRenderStateDirty();
	}
}
