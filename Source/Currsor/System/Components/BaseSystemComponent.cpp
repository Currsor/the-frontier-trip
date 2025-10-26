// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseSystemComponent.h"

UBaseSystemComponent::UBaseSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBaseSystemComponent::BeginPlay()
{
    Super::BeginPlay();

    if (bAutoInitializeOnBeginPlay && !bIsInitialized)
    {
        Initialize();
    }
}

void UBaseSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bIsInitialized)
    {
        Shutdown();
    }

    Super::EndPlay(EndPlayReason);
}

void UBaseSystemComponent::Initialize()
{
    if (bIsInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s 已初始化"), *SystemName);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("正在初始化 %s..."), *SystemName);

    // 调用子类实现
    OnInitialize();

    bIsInitialized = true;
    UE_LOG(LogTemp, Log, TEXT("%s 初始化成功"), *SystemName);
}

void UBaseSystemComponent::Reset()
{
    if (!bIsInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("无法重置 %s - 未初始化"), *SystemName);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("正在重置 %s..."), *SystemName);

    // 调用子类实现
    OnReset();

    UE_LOG(LogTemp, Log, TEXT("%s 重置完成"), *SystemName);
}

void UBaseSystemComponent::Shutdown()
{
    if (!bIsInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("无法关闭 %s - 未初始化"), *SystemName);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("正在关闭 %s..."), *SystemName);

    // 调用子类实现
    OnShutdown();

    bIsInitialized = false;
    UE_LOG(LogTemp, Log, TEXT("%s 关闭完成"), *SystemName);
}

void UBaseSystemComponent::DebugPrintStatus() const
{
    UE_LOG(LogTemp, Log, TEXT("=== %s 状态 ==="), *SystemName);
    UE_LOG(LogTemp, Log, TEXT("已初始化: %s"), bIsInitialized ? TEXT("是") : TEXT("否"));
    UE_LOG(LogTemp, Log, TEXT("所有者: %s"), GetOwner() ? *GetOwner()->GetName() : TEXT("无"));
    UE_LOG(LogTemp, Log, TEXT("========================"));
}