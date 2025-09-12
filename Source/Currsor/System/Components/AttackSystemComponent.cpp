// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackSystemComponent.h"
#include "Engine/World.h"
#include "Currsor/Interface/IDamageable.h"
#include "Kismet/KismetMathLibrary.h"

UAttackSystemComponent::UAttackSystemComponent()
{
    SystemName = TEXT("AttackSystem");
    PrimaryComponentTick.bCanEverTick = false;
}

void UAttackSystemComponent::OnInitialize()
{
    Super::OnInitialize();
    
    // 清理数据
    ActiveAttacks.Empty();
    LastAttackTimes.Empty();
    TotalAttacksProcessed = 0;
    TotalDamageDealt = 0;

    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("AttackSystem initialized with global damage multiplier: %f"), GlobalDamageMultiplier);
    }
}

void UAttackSystemComponent::OnReset()
{
    Super::OnReset();
    
    // 清理活跃攻击
    ActiveAttacks.Empty();
    LastAttackTimes.Empty();
    
    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("AttackSystem reset - cleared %d active attacks"), ActiveAttacks.Num());
    }
}

void UAttackSystemComponent::OnShutdown()
{
    Super::OnShutdown();
    
    // 清理所有数据
    ActiveAttacks.Empty();
    LastAttackTimes.Empty();
    
    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("AttackSystem shutdown - Total attacks processed: %d, Total damage dealt: %d"), 
               TotalAttacksProcessed, TotalDamageDealt);
    }
}

bool UAttackSystemComponent::ProcessAttack(AActor* Attacker, AActor* Target, const FAttackData& AttackData)
{
    if (!bIsInitialized)
    {
        UE_LOG(LogTemp, Error, TEXT("AttackSystem not initialized"));
        return false;
    }

    if (!Attacker || !Target)
    {
        UE_LOG(LogTemp, Error, TEXT("ProcessAttack: Invalid attacker or target"));
        return false;
    }

    if (!CanAttack(Attacker))
    {
        if (bEnableDebugLogging)
        {
            UE_LOG(LogTemp, Warning, TEXT("ProcessAttack: %s cannot attack (cooldown or already attacking)"), 
                   *Attacker->GetName());
        }
        return false;
    }

    // 计算伤害
    bool bIsCritical = false;
    float FinalDamage = CalculateDamage(AttackData, bIsCritical);

    // 应用伤害
    if (ApplyDamageToTarget(Target, FinalDamage, Attacker))
    {
        // 更新统计
        TotalAttacksProcessed++;
        TotalDamageDealt += FinalDamage;

        // 更新最后攻击时间
        LastAttackTimes.Add(Attacker, GetWorld()->GetTimeSeconds());

        // 广播攻击命中事件
        BroadcastAttackHit(Attacker, Target, FinalDamage);

        if (bEnableDebugLogging)
        {
            UE_LOG(LogTemp, Log, TEXT("Attack processed: %s -> %s, Damage: %f %s"), 
                   *Attacker->GetName(), *Target->GetName(), FinalDamage, 
                   bIsCritical ? TEXT("(CRITICAL)") : TEXT(""));
        }

        return true;
    }

    return false;
}

float UAttackSystemComponent::CalculateDamage(const FAttackData& AttackData, bool& bIsCritical)
{
    float Damage = AttackData.BaseDamage * GlobalDamageMultiplier;

    // 检查暴击
    bIsCritical = FMath::RandRange(0.0f, 1.0f) <= AttackData.CriticalChance;
    if (bIsCritical)
    {
        Damage *= AttackData.CriticalMultiplier;
    }

    return FMath::Max(0.0f, Damage);
}

bool UAttackSystemComponent::CanAttack(AActor* Attacker) const
{
    if (!Attacker)
    {
        return false;
    }

    // 检查是否正在攻击
    if (IsAttacking(Attacker))
    {
        return false;
    }

    // 检查攻击冷却
    if (const float* LastAttackTime = LastAttackTimes.Find(Attacker))
    {
        float CurrentTime = GetWorld()->GetTimeSeconds();
        if (CurrentTime - *LastAttackTime < AttackCooldown)
        {
            return false;
        }
    }

    return true;
}

void UAttackSystemComponent::StartAttack(AActor* Attacker, const FString& AttackType)
{
    if (!Attacker)
    {
        return;
    }

    ActiveAttacks.Add(Attacker);
    OnAttackStarted.Broadcast(Attacker, AttackType);

    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("Attack started: %s (%s)"), *Attacker->GetName(), *AttackType);
    }
}

void UAttackSystemComponent::EndAttack(AActor* Attacker)
{
    if (!Attacker)
    {
        return;
    }

    ActiveAttacks.Remove(Attacker);
    OnAttackEnd.Broadcast(Attacker);

    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("Attack ended: %s"), *Attacker->GetName());
    }
}

bool UAttackSystemComponent::IsAttacking(AActor* Attacker) const
{
    if (!Attacker)
    {
        return false;
    }

    return ActiveAttacks.Contains(Attacker);
}

bool UAttackSystemComponent::ApplyDamageToTarget(AActor* Target, float Damage, AActor* Instigator)
{
    if (!Target)
    {
        return false;
    }

    // 尝试通过IDamageable接口应用伤害
    if (Target->Implements<UDamageable>())
    {
        FHitResult HitResult;
        HitResult.HitObjectHandle = FActorInstanceHandle(Target);
        HitResult.Location = Target->GetActorLocation();
        HitResult.ImpactPoint = Target->GetActorLocation();

        IDamageable::Execute_ApplyDamage(Target, Damage, Instigator, HitResult);
        return true;
    }

    // 如果目标没有实现IDamageable接口，记录警告
    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Warning, TEXT("Target %s does not implement IDamageable interface"), 
               *Target->GetName());
    }

    return false;
}

void UAttackSystemComponent::BroadcastAttackHit(AActor* Attacker, AActor* Target, float Damage)
{
    OnAttackHit.Broadcast(Attacker, Target, Damage);
}