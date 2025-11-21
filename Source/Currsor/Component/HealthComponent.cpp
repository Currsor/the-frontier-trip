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
		UE_LOG(LogTemp, Warning, TEXT("最大生命值必须大于0"));
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

void UHealthComponent::SetDefense(float NewDefense)
{
    Defense = FMath::Max(0.0f, NewDefense);
}

float UHealthComponent::CalculateDamageReduction(float IncomingDamage) const
{
    // 此方法现在返回扣除防御后剩余的伤害值
    // 如果防御值足够，返回0；否则返回溢出的伤害
    if (IncomingDamage <= 0.0f)
    {
        return 0.0f;
    }

    if (Defense >= IncomingDamage)
    {
        // 防御值足够吸收所有伤害
        return 0.0f;
    }
    else
    {
        // 防御值不足，返回溢出的伤害
        return IncomingDamage - Defense;
    }
}

void UHealthComponent::TakeDamage(float DamageAmount, AActor* DamageInstigator)
{
    if (!bCanTakeDamage || IsDead() || DamageAmount <= 0.0f)
    {
        return;
    }

    float RemainingDamage = DamageAmount;
    float PreviousDefense = Defense;
    float PreviousHealth = CurrentHealth;

    // 优先扣除防御值（护盾）
    if (Defense > 0.0f)
    {
        if (Defense >= RemainingDamage)
        {
            // 防御值足够吸收所有伤害
            Defense -= RemainingDamage;
            RemainingDamage = 0.0f;
        }
        else
        {
            // 防御值不足，扣光防御值后剩余伤害继续扣生命值
            RemainingDamage -= Defense;
            Defense = 0.0f;
        }
    }

    // 如果还有剩余伤害，扣除生命值
    if (RemainingDamage > 0.0f)
    {
        CurrentHealth = FMath::Clamp(CurrentHealth - RemainingDamage, 0.0f, MaxHealth);
    }

    // 只要防御值或生命值有变化就广播事件
    if (Defense != PreviousDefense || CurrentHealth != PreviousHealth)
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

void UHealthComponent::UseMana(int32 ManaCost)
{
    if (ManaCount > 0.0f)
    {
        ManaCount = FMath::Clamp(ManaCount - ManaCost, 0.0f, MaxManaCount);
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