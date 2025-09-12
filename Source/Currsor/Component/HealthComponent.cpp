// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    CurrentHealth = MaxHealth;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // 确保当前生命值不超过最大生命值
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
}

void UHealthComponent::SetMaxHealth(float NewMaxHealth)
{
    if (NewMaxHealth <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("MaxHealth must be greater than 0"));
        return;
    }

    MaxHealth = NewMaxHealth;
    
    // 如果当前生命值超过新的最大值，则调整
    if (CurrentHealth > MaxHealth)
    {
        CurrentHealth = MaxHealth;
        BroadcastHealthChanged();
    }
}

float UHealthComponent::GetHealthPercentage() const
{
    return MaxHealth > 0.0f ? (CurrentHealth / MaxHealth) : 0.0f;
}

void UHealthComponent::TakeDamage(float DamageAmount, AActor* DamageInstigator)
{
    if (!bCanTakeDamage || IsDead() || DamageAmount <= 0.0f)
    {
        return;
    }

    float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

    // 只有生命值真正改变时才广播事件
    if (CurrentHealth != PreviousHealth)
    {
        BroadcastHealthChanged(DamageAmount);

        // 检查是否死亡
        if (IsDead())
        {
            HandleDeath();
        }
    }
}

void UHealthComponent::Heal(float HealAmount)
{
    if (IsDead() || HealAmount <= 0.0f)
    {
        return;
    }

    float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);

    if (CurrentHealth != PreviousHealth)
    {
        BroadcastHealthChanged(-HealAmount); // 负值表示治疗
    }
}

void UHealthComponent::SetCurrentHealth(float NewHealth)
{
    float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);

    if (CurrentHealth != PreviousHealth)
    {
        BroadcastHealthChanged();

        if (IsDead() && PreviousHealth > 0.0f)
        {
            HandleDeath();
        }
    }
}

void UHealthComponent::HandleDeath()
{
    bCanTakeDamage = false;
    
    // 广播死亡事件
    OnDeath.Broadcast(GetOwner());

    // 自动重生逻辑
    if (bAutoRespawn && RespawnDelay > 0.0f)
    {
        FTimerHandle RespawnTimer;
        GetWorld()->GetTimerManager().SetTimer(RespawnTimer, [this]()
        {
            CurrentHealth = MaxHealth;
            bCanTakeDamage = true;
            BroadcastHealthChanged();
        }, RespawnDelay, false);
    }
}

void UHealthComponent::BroadcastHealthChanged(float DamageAmount)
{
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, DamageAmount);
}