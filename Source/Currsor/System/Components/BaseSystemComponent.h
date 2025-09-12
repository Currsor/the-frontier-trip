// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseSystemComponent.generated.h"

/**
 * 系统组件基类
 * 为所有游戏系统组件提供统一的接口和生命周期管理
 */
UCLASS(Abstract, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CURRSOR_API UBaseSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBaseSystemComponent();

    // 系统生命周期
    UFUNCTION(BlueprintCallable, Category = "System")
    virtual void Initialize();

    UFUNCTION(BlueprintCallable, Category = "System")
    virtual void Reset();

    UFUNCTION(BlueprintCallable, Category = "System")
    virtual void Shutdown();

    // 状态查询
    UFUNCTION(BlueprintPure, Category = "System")
    bool IsSystemInitialized() const { return bIsInitialized; }

    UFUNCTION(BlueprintPure, Category = "System")
    FString GetSystemName() const { return SystemName; }

    // 调试
    UFUNCTION(BlueprintCallable, Category = "System")
    virtual void DebugPrintStatus() const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // 子类需要实现的虚函数
    virtual void OnInitialize() {}
    virtual void OnReset() {}
    virtual void OnShutdown() {}

    // 系统状态
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "System")
    bool bIsInitialized = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "System")
    FString SystemName = TEXT("BaseSystem");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System")
    bool bAutoInitializeOnBeginPlay = true;
};