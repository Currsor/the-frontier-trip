// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSystemComponent.h"
#include "Currsor/Character/Component/BaseState.h"
#include "StateManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStateChanged, AActor*, Actor, ECharacterState, NewState, ECharacterState, OldState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateTransitionFailed, AActor*, Actor, ECharacterState, AttemptedState);

USTRUCT(BlueprintType)
struct FStateTransitionRule
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Transition")
    ECharacterState FromState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Transition")
    ECharacterState ToState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Transition")
    bool bIsAllowed = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Transition")
    float MinDuration = 0.0f;

    FStateTransitionRule()
    {
        FromState = ECharacterState::Idle;
        ToState = ECharacterState::Idle;
        bIsAllowed = true;
        MinDuration = 0.0f;
    }
};

// 状态数据结构
USTRUCT()
struct FActorStateData
{
    GENERATED_BODY()

    UPROPERTY()
    ECharacterState CurrentState = ECharacterState::Idle;

    UPROPERTY()
    ECharacterState PreviousState = ECharacterState::Idle;

    UPROPERTY()
    float StateStartTime = 0.0f;

    UPROPERTY()
    float LastTransitionTime = 0.0f;
};

/**
 * 状态管理器组件
 * 管理角色状态转换、验证和优先级
 * 对应TypeScript中的StateManager
 */
UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CURRSOR_API UStateManagerComponent : public UBaseSystemComponent
{
    GENERATED_BODY()

public:
    UStateManagerComponent();

    // 状态管理
    UFUNCTION(BlueprintCallable, Category = "State Manager")
    bool ChangeState(AActor* Actor, ECharacterState NewState, bool bForceChange = false);

    UFUNCTION(BlueprintCallable, Category = "State Manager")
    bool CanTransitionTo(AActor* Actor, ECharacterState NewState) const;

    UFUNCTION(BlueprintPure, Category = "State Manager")
    ECharacterState GetCurrentState(AActor* Actor) const;

    UFUNCTION(BlueprintPure, Category = "State Manager")
    ECharacterState GetPreviousState(AActor* Actor) const;

    // 状态优先级
    UFUNCTION(BlueprintPure, Category = "State Manager")
    int32 GetStatePriority(ECharacterState State) const;

    UFUNCTION(BlueprintCallable, Category = "State Manager")
    void SetStatePriority(ECharacterState State, int32 Priority);

    // 状态持续时间
    UFUNCTION(BlueprintPure, Category = "State Manager")
    float GetStateElapsedTime(AActor* Actor) const;

    UFUNCTION(BlueprintPure, Category = "State Manager")
    bool HasStateMinDurationPassed(AActor* Actor) const;

    // 转换规则
    UFUNCTION(BlueprintCallable, Category = "State Manager")
    void AddTransitionRule(const FStateTransitionRule& Rule);

    UFUNCTION(BlueprintCallable, Category = "State Manager")
    void RemoveTransitionRule(ECharacterState FromState, ECharacterState ToState);

    UFUNCTION(BlueprintCallable, Category = "State Manager")
    void ClearTransitionRules();

    // 状态查询
    UFUNCTION(BlueprintPure, Category = "State Manager")
    bool IsInState(AActor* Actor, ECharacterState State) const;

    UFUNCTION(BlueprintPure, Category = "State Manager")
    bool IsInAnyState(AActor* Actor, const TArray<ECharacterState>& States) const;

    UFUNCTION(BlueprintPure, Category = "State Manager")
    int32 GetManagedActorCount() const { return ActorStates.Num(); }

    // 事件
    UPROPERTY(BlueprintAssignable, Category = "State Manager")
    FOnStateChanged OnStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "State Manager")
    FOnStateTransitionFailed OnStateTransitionFailed;

protected:
    virtual void OnInitialize() override;
    virtual void OnReset() override;
    virtual void OnShutdown() override;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

    // 内部函数
    void InitializeDefaultPriorities();
    void InitializeDefaultTransitionRules();
    bool ValidateTransition(AActor* Actor, ECharacterState FromState, ECharacterState ToState) const;
    void BroadcastStateChange(AActor* Actor, ECharacterState NewState, ECharacterState OldState);

    // 数据存储
    UPROPERTY(VisibleAnywhere, Category = "State Manager")
    TMap<TWeakObjectPtr<AActor>, FActorStateData> ActorStates;

    UPROPERTY(EditAnywhere, Category = "State Manager")
    TMap<ECharacterState, int32> StatePriorities;

    UPROPERTY(EditAnywhere, Category = "State Manager")
    TArray<FStateTransitionRule> TransitionRules;

    // 配置
    UPROPERTY(EditAnywhere, Category = "State Manager")
    bool bEnableDebugLogging = true;

    UPROPERTY(EditAnywhere, Category = "State Manager")
    bool bEnableStateTicking = true;

    UPROPERTY(EditAnywhere, Category = "State Manager")
    float DefaultMinStateDuration = 0.1f;
};