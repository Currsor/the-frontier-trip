// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackSystemComponent.h"
#include "Engine/World.h"
#include "Currsor/Interface/IDamageable.h"
#include "Kismet/KismetMathLibrary.h"
#include "BattleAreaTeleportComponent.h"
#include "Currsor/Character/Player/CurrsorCharacter.h"
#include "Currsor/Character/Enemy/BaseEnemy.h"

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
        UE_LOG(LogTemp, Log, TEXT("攻击系统已初始化，全局伤害倍数: %f"), GlobalDamageMultiplier);
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
        UE_LOG(LogTemp, Log, TEXT("攻击系统已重置 - 清除了 %d 个活跃攻击"), ActiveAttacks.Num());
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
        UE_LOG(LogTemp, Log, TEXT("攻击系统已关闭 - 处理的攻击总数: %d, 造成的伤害总量: %d"), 
               TotalAttacksProcessed, TotalDamageDealt);
    }
}

bool UAttackSystemComponent::ProcessAttack(AActor* Attacker, AActor* Target, const FAttackData& AttackData)
{
    if (!bIsInitialized)
    {
        UE_LOG(LogTemp, Error, TEXT("攻击系统未初始化"));
        return false;
    }

    if (!Attacker || !Target)
    {
		UE_LOG(LogTemp, Error, TEXT("ProcessAttack: 攻击者或目标无效"));
        return false;
    }

    // 检查对象有效性
    if (!IsValid(Attacker) || !IsValid(Target))
    {
		UE_LOG(LogTemp, Error, TEXT("ProcessAttack: 攻击者或目标无效"));
        return false;
    }

    if (!CanAttack(Attacker, true))
    {
        if (bEnableDebugLogging)
        {
            FString AttackerName = IsValid(Attacker) ? Attacker->GetName() : TEXT("Invalid");
            UE_LOG(LogTemp, Warning, TEXT("ProcessAttack: %s 无法攻击（冷却中）"), 
                   *AttackerName);
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
        UWorld* World = GetWorld();
        if (World)
        {
            LastAttackTimes.Add(Attacker, World->GetTimeSeconds());
        }

        // 广播攻击命中事件
        BroadcastAttackHit(Attacker, Target, FinalDamage);

        // 处理战斗区域传送
        ProcessBattleAreaTeleportIfNeeded(Attacker, Target);

        if (bEnableDebugLogging)
        {
            FString AttackerName = IsValid(Attacker) ? Attacker->GetName() : TEXT("Invalid");
            FString TargetName = IsValid(Target) ? Target->GetName() : TEXT("Invalid");
            UE_LOG(LogTemp, Log, TEXT("攻击已处理: %s -> %s, 伤害: %f %s"), 
                   *AttackerName, *TargetName, FinalDamage, 
                   bIsCritical ? TEXT("(暴击)") : TEXT(""));
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

bool UAttackSystemComponent::ProcessAttackInput(AActor* Attacker)
{
    if (!bIsInitialized)
    {
        UE_LOG(LogTemp, Error, TEXT("攻击系统未初始化"));
        return false;
    }

    if (!Attacker)
    {
        UE_LOG(LogTemp, Error, TEXT("ProcessAttackInput: 攻击者无效"));
        return false;
    }

    if (!CanAttack(Attacker))
    {
        if (bEnableDebugLogging)
        {
            FString AttackerName = IsValid(Attacker) ? Attacker->GetName() : TEXT("Invalid");
            UE_LOG(LogTemp, Warning, TEXT("ProcessAttackInput: %s 无法攻击（冷却中或正在攻击）"), 
                   *AttackerName);
        }
        return false;
    }

    // 开始攻击状态
    StartAttack(Attacker, TEXT("Normal"));

    if (bEnableDebugLogging)
    {
        FString AttackerName = IsValid(Attacker) ? Attacker->GetName() : TEXT("Invalid");
        UE_LOG(LogTemp, Log, TEXT("已处理攻击输入: %s"), *AttackerName);
    }

    return true;
}

bool UAttackSystemComponent::ProcessAttackHit(AActor* Attacker, AActor* Target, const FHitResult& HitResult)
{
    if (!bIsInitialized)
    {
        UE_LOG(LogTemp, Error, TEXT("攻击系统未初始化"));
        return false;
    }

    if (!Attacker || !Target)
    {
        UE_LOG(LogTemp, Error, TEXT("ProcessAttackHit: 攻击者或目标无效"));
        return false;
    }

    // 移除攻击状态检查 - 碰撞检测本身就表明攻击是有效的
    // ProcessAttack函数内部会处理冷却时间等其他检查
    
    if (bEnableDebugLogging)
    {
        FString AttackerName = IsValid(Attacker) ? Attacker->GetName() : TEXT("Invalid");
        FString TargetName = IsValid(Target) ? Target->GetName() : TEXT("Invalid");
        UE_LOG(LogTemp, Log, TEXT("ProcessAttackHit: %s 命中 %s"), 
               *AttackerName, *TargetName);
    }

    // 使用默认攻击数据处理命中
    FAttackData DefaultAttackData;
    return ProcessAttack(Attacker, Target, DefaultAttackData);
}

bool UAttackSystemComponent::CanAttack(AActor* Attacker, bool bAllowDuringAttack) const
{
    if (!Attacker || !IsValid(Attacker))
    {
        return false;
    }

    // 检查是否正在攻击（如果允许攻击状态中的检查，则跳过此检查）
    if (!bAllowDuringAttack && IsAttacking(Attacker))
    {
        return false;
    }

    // 检查攻击冷却
    if (const float* LastAttackTime = LastAttackTimes.Find(Attacker))
    {
        UWorld* World = GetWorld();
        if (!World)
        {
            UE_LOG(LogTemp, Error, TEXT("CanAttack: GetWorld() 返回空值"));
            return false;
        }
        
        float CurrentTime = World->GetTimeSeconds();
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
        FString AttackerName = IsValid(Attacker) ? Attacker->GetName() : TEXT("Invalid");
        UE_LOG(LogTemp, Log, TEXT("攻击开始: %s (%s)"), *AttackerName, *AttackType);
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
        FString AttackerName = IsValid(Attacker) ? Attacker->GetName() : TEXT("Invalid");
        UE_LOG(LogTemp, Log, TEXT("攻击结束: %s"), *AttackerName);
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
        FString TargetName = IsValid(Target) ? Target->GetName() : TEXT("Invalid");
        UE_LOG(LogTemp, Warning, TEXT("目标 %s 未实现 IDamageable 接口"), 
               *TargetName);
    }

    return false;
}

void UAttackSystemComponent::BroadcastAttackHit(AActor* Attacker, AActor* Target, float Damage)
{
    OnAttackHit.Broadcast(Attacker, Target, Damage);

    if (bEnableDebugLogging)
    {
        FString AttackerName = IsValid(Attacker) ? Attacker->GetName() : TEXT("Invalid");
        FString TargetName = IsValid(Target) ? Target->GetName() : TEXT("Invalid");
        UE_LOG(LogTemp, Log, TEXT("攻击命中广播: %s -> %s, 伤害: %f"), 
               *AttackerName, *TargetName, Damage);
    }
}

void UAttackSystemComponent::SetBattleAreaTeleportComponent(UBattleAreaTeleportComponent* TeleportComponent)
{
    BattleAreaTeleportComponent = TeleportComponent;
    
    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("BattleAreaTeleportComponent 设置: %s"), 
               TeleportComponent ? TEXT("有效") : TEXT("空值"));
    }
}

void UAttackSystemComponent::ProcessBattleAreaTeleportIfNeeded(AActor* Attacker, AActor* Target)
{
    if (!bEnableBattleAreaTeleport || !BattleAreaTeleportComponent)
    {
        return;
    }

    // 检查攻击者是否为玩家角色
    ACurrsorCharacter* Player = Cast<ACurrsorCharacter>(Attacker);
    if (!Player)
    {
        return;
    }

    // 检查目标是否为BaseEnemy
    ABaseEnemy* Enemy = Cast<ABaseEnemy>(Target);
    if (!Enemy)
    {
        return;
    }

    // 执行战斗区域传送
    bool bTeleportSuccess = BattleAreaTeleportComponent->ProcessBattleAreaTeleport(Player, Enemy);
    
    if (bEnableDebugLogging)
    {
        FString AttackerName = IsValid(Attacker) ? Attacker->GetName() : TEXT("Invalid");
        FString TargetName = IsValid(Target) ? Target->GetName() : TEXT("Invalid");
        UE_LOG(LogTemp, Log, TEXT("战斗区域传送 %s 于 %s -> %s"), 
               bTeleportSuccess ? TEXT("成功") : TEXT("失败"),
               *AttackerName, *TargetName);
    }
}