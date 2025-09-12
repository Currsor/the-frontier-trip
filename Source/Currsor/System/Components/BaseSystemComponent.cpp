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
        UE_LOG(LogTemp, Warning, TEXT("%s already initialized"), *SystemName);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Initializing %s..."), *SystemName);

    // 调用子类实现
    OnInitialize();

    bIsInitialized = true;
    UE_LOG(LogTemp, Log, TEXT("%s initialized successfully"), *SystemName);
}

void UBaseSystemComponent::Reset()
{
    if (!bIsInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot reset %s - not initialized"), *SystemName);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Resetting %s..."), *SystemName);

    // 调用子类实现
    OnReset();

    UE_LOG(LogTemp, Log, TEXT("%s reset complete"), *SystemName);
}

void UBaseSystemComponent::Shutdown()
{
    if (!bIsInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot shutdown %s - not initialized"), *SystemName);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Shutting down %s..."), *SystemName);

    // 调用子类实现
    OnShutdown();

    bIsInitialized = false;
    UE_LOG(LogTemp, Log, TEXT("%s shutdown complete"), *SystemName);
}

void UBaseSystemComponent::DebugPrintStatus() const
{
    UE_LOG(LogTemp, Log, TEXT("=== %s Status ==="), *SystemName);
    UE_LOG(LogTemp, Log, TEXT("Initialized: %s"), bIsInitialized ? TEXT("True") : TEXT("False"));
    UE_LOG(LogTemp, Log, TEXT("Owner: %s"), GetOwner() ? *GetOwner()->GetName() : TEXT("None"));
    UE_LOG(LogTemp, Log, TEXT("========================"));
}