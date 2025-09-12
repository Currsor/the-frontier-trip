// Fill out your copyright notice in the Description page of Project Settings.

#include "StateManagerComponent.h"
#include "Engine/World.h"

UStateManagerComponent::UStateManagerComponent()
{
    SystemName = TEXT("StateManager");
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UStateManagerComponent::OnInitialize()
{
    Super::OnInitialize();
    
    // 初始化默认优先级和转换规则
    InitializeDefaultPriorities();
    InitializeDefaultTransitionRules();
    
    // 清理数据
    ActorStates.Empty();
    
    // 启用Tick如果需要
    if (bEnableStateTicking)
    {
        SetComponentTickEnabled(true);
    }

    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("StateManager initialized with %d priorities and %d transition rules"), 
               StatePriorities.Num(), TransitionRules.Num());
    }
}

void UStateManagerComponent::OnReset()
{
    Super::OnReset();
    
    // 清理所有Actor状态
    ActorStates.Empty();
    
    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("StateManager reset - cleared all actor states"));
    }
}

void UStateManagerComponent::OnShutdown()
{
    Super::OnShutdown();
    
    // 停用Tick
    SetComponentTickEnabled(false);
    
    // 清理数据
    ActorStates.Empty();
    
    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("StateManager shutdown"));
    }
}

void UStateManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (!bIsInitialized || !bEnableStateTicking)
    {
        return;
    }

    // 清理无效的Actor引用
    for (auto It = ActorStates.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid())
        {
            It.RemoveCurrent();
        }
    }
}

bool UStateManagerComponent::ChangeState(AActor* Actor, ECharacterState NewState, bool bForceChange)
{
    if (!bIsInitialized)
    {
        UE_LOG(LogTemp, Error, TEXT("StateManager not initialized"));
        return false;
    }

    if (!Actor)
    {
        UE_LOG(LogTemp, Error, TEXT("ChangeState: Invalid actor"));
        return false;
    }

    // 获取或创建Actor状态数据
    FActorStateData* StateData = ActorStates.Find(Actor);
    if (!StateData)
    {
        FActorStateData NewStateData;
        NewStateData.CurrentState = ECharacterState::Idle;
        NewStateData.PreviousState = ECharacterState::Idle;
        NewStateData.StateStartTime = GetWorld()->GetTimeSeconds();
        NewStateData.LastTransitionTime = GetWorld()->GetTimeSeconds();
        
        ActorStates.Add(Actor, NewStateData);
        StateData = ActorStates.Find(Actor);
    }

    ECharacterState CurrentState = StateData->CurrentState;

    // 检查是否已经在目标状态
    if (CurrentState == NewState && !bForceChange)
    {
        return true;
    }

    // 验证转换
    if (!bForceChange && !ValidateTransition(Actor, CurrentState, NewState))
    {
        OnStateTransitionFailed.Broadcast(Actor, NewState);
        
        if (bEnableDebugLogging)
        {
            UE_LOG(LogTemp, Warning, TEXT("State transition failed: %s -> %s for %s"), 
                   *UEnum::GetValueAsString(CurrentState), 
                   *UEnum::GetValueAsString(NewState), 
                   *Actor->GetName());
        }
        return false;
    }

    // 执行状态转换
    StateData->PreviousState = CurrentState;
    StateData->CurrentState = NewState;
    StateData->StateStartTime = GetWorld()->GetTimeSeconds();
    StateData->LastTransitionTime = GetWorld()->GetTimeSeconds();

    // 广播状态变化事件
    BroadcastStateChange(Actor, NewState, CurrentState);

    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("State changed: %s -> %s for %s"), 
               *UEnum::GetValueAsString(CurrentState), 
               *UEnum::GetValueAsString(NewState), 
               *Actor->GetName());
    }

    return true;
}

bool UStateManagerComponent::CanTransitionTo(AActor* Actor, ECharacterState NewState) const
{
    if (!Actor)
    {
        return false;
    }

    ECharacterState CurrentState = GetCurrentState(Actor);
    return ValidateTransition(Actor, CurrentState, NewState);
}

ECharacterState UStateManagerComponent::GetCurrentState(AActor* Actor) const
{
    if (!Actor)
    {
        return ECharacterState::Idle;
    }

    const FActorStateData* StateData = ActorStates.Find(Actor);
    return StateData ? StateData->CurrentState : ECharacterState::Idle;
}

ECharacterState UStateManagerComponent::GetPreviousState(AActor* Actor) const
{
    if (!Actor)
    {
        return ECharacterState::Idle;
    }

    const FActorStateData* StateData = ActorStates.Find(Actor);
    return StateData ? StateData->PreviousState : ECharacterState::Idle;
}

int32 UStateManagerComponent::GetStatePriority(ECharacterState State) const
{
    const int32* Priority = StatePriorities.Find(State);
    return Priority ? *Priority : 0;
}

void UStateManagerComponent::SetStatePriority(ECharacterState State, int32 Priority)
{
    StatePriorities.Add(State, Priority);
}

float UStateManagerComponent::GetStateElapsedTime(AActor* Actor) const
{
    if (!Actor)
    {
        return 0.0f;
    }

    const FActorStateData* StateData = ActorStates.Find(Actor);
    if (!StateData)
    {
        return 0.0f;
    }

    return GetWorld()->GetTimeSeconds() - StateData->StateStartTime;
}

bool UStateManagerComponent::HasStateMinDurationPassed(AActor* Actor) const
{
    return GetStateElapsedTime(Actor) >= DefaultMinStateDuration;
}

void UStateManagerComponent::AddTransitionRule(const FStateTransitionRule& Rule)
{
    // 移除现有的相同规则
    RemoveTransitionRule(Rule.FromState, Rule.ToState);
    
    // 添加新规则
    TransitionRules.Add(Rule);
}

void UStateManagerComponent::RemoveTransitionRule(ECharacterState FromState, ECharacterState ToState)
{
    TransitionRules.RemoveAll([FromState, ToState](const FStateTransitionRule& Rule)
    {
        return Rule.FromState == FromState && Rule.ToState == ToState;
    });
}

void UStateManagerComponent::ClearTransitionRules()
{
    TransitionRules.Empty();
}

bool UStateManagerComponent::IsInState(AActor* Actor, ECharacterState State) const
{
    return GetCurrentState(Actor) == State;
}

bool UStateManagerComponent::IsInAnyState(AActor* Actor, const TArray<ECharacterState>& States) const
{
    ECharacterState CurrentState = GetCurrentState(Actor);
    return States.Contains(CurrentState);
}

void UStateManagerComponent::InitializeDefaultPriorities()
{
    // 设置默认状态优先级（数值越高优先级越高）
    StatePriorities.Add(ECharacterState::Dead, 100);
    StatePriorities.Add(ECharacterState::Hurt, 90);
    StatePriorities.Add(ECharacterState::Dash, 80);
    StatePriorities.Add(ECharacterState::Attack, 70);
    StatePriorities.Add(ECharacterState::RunAttack, 75);
    StatePriorities.Add(ECharacterState::Jump, 60);
    StatePriorities.Add(ECharacterState::Fall, 50);
    StatePriorities.Add(ECharacterState::Run, 30);
    StatePriorities.Add(ECharacterState::Walk, 20);
    StatePriorities.Add(ECharacterState::Idle, 10);
}

void UStateManagerComponent::InitializeDefaultTransitionRules()
{
    // 清空现有规则
    TransitionRules.Empty();
    
    // 添加基本转换规则
    // 死亡状态不能转换到其他状态
    for (int32 i = 0; i < (int32)ECharacterState::Dead; ++i)
    {
        FStateTransitionRule Rule;
        Rule.FromState = ECharacterState::Dead;
        Rule.ToState = (ECharacterState)i;
        Rule.bIsAllowed = false;
        TransitionRules.Add(Rule);
    }
    
    // 攻击状态需要最小持续时间
    FStateTransitionRule AttackRule;
    AttackRule.FromState = ECharacterState::Attack;
    AttackRule.ToState = ECharacterState::Idle;
    AttackRule.MinDuration = 0.3f;
    TransitionRules.Add(AttackRule);
}

bool UStateManagerComponent::ValidateTransition(AActor* Actor, ECharacterState FromState, ECharacterState ToState) const
{
    // 检查优先级
    int32 FromPriority = GetStatePriority(FromState);
    int32 ToPriority = GetStatePriority(ToState);
    
    // 高优先级状态不能被低优先级状态打断（除非是特殊情况）
    if (FromPriority > ToPriority && FromState != ECharacterState::Idle)
    {
        // 检查是否有特殊规则允许这种转换
        bool bHasAllowingRule = false;
        for (const FStateTransitionRule& Rule : TransitionRules)
        {
            if (Rule.FromState == FromState && Rule.ToState == ToState && Rule.bIsAllowed)
            {
                bHasAllowingRule = true;
                break;
            }
        }
        
        if (!bHasAllowingRule)
        {
            return false;
        }
    }

    // 检查转换规则
    for (const FStateTransitionRule& Rule : TransitionRules)
    {
        if (Rule.FromState == FromState && Rule.ToState == ToState)
        {
            if (!Rule.bIsAllowed)
            {
                return false;
            }
            
            // 检查最小持续时间
            if (Rule.MinDuration > 0.0f)
            {
                float ElapsedTime = GetStateElapsedTime(Actor);
                if (ElapsedTime < Rule.MinDuration)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

void UStateManagerComponent::BroadcastStateChange(AActor* Actor, ECharacterState NewState, ECharacterState OldState)
{
    OnStateChanged.Broadcast(Actor, NewState, OldState);
}