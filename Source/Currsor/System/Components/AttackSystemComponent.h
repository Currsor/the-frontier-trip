// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSystemComponent.h"
#include "Engine/HitResult.h"
#include "AttackSystemComponent.generated.h"

class AActor;
class IDamageable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttackHit, AActor*, Attacker, AActor*, Target, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttackStarted, AActor*, Attacker, FString, AttackType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackEnd, AActor*, Attacker);

USTRUCT(BlueprintType)
struct FAttackData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float BaseDamage = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float CriticalChance = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float CriticalMultiplier = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    FString AttackType = TEXT("Normal");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    bool bCanInterrupt = true;

    FAttackData()
    {
        BaseDamage = 10.0f;
        CriticalChance = 0.1f;
        CriticalMultiplier = 2.0f;
        AttackType = TEXT("Normal");
        bCanInterrupt = true;
    }
};

/**
 * 攻击系统组件
 * 处理攻击逻辑、伤害计算和攻击事件
 * 对应TypeScript中的AttackSystem
 */
UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CURRSOR_API UAttackSystemComponent : public UBaseSystemComponent
{
    GENERATED_BODY()

public:
    UAttackSystemComponent();

    // 攻击处理
    UFUNCTION(BlueprintCallable, Category = "Attack System")
    bool ProcessAttack(AActor* Attacker, AActor* Target, const FAttackData& AttackData = FAttackData());

    UFUNCTION(BlueprintCallable, Category = "Attack System")
    float CalculateDamage(const FAttackData& AttackData, bool& bIsCritical);

    UFUNCTION(BlueprintCallable, Category = "Attack System")
    bool CanAttack(AActor* Attacker) const;

    UFUNCTION(BlueprintCallable, Category = "Attack System")
    void StartAttack(AActor* Attacker, const FString& AttackType = TEXT("Normal"));

    UFUNCTION(BlueprintCallable, Category = "Attack System")
    void EndAttack(AActor* Attacker);

    // 攻击状态查询
    UFUNCTION(BlueprintPure, Category = "Attack System")
    bool IsAttacking(AActor* Attacker) const;

    UFUNCTION(BlueprintPure, Category = "Attack System")
    int32 GetActiveAttackCount() const { return ActiveAttacks.Num(); }

    // 配置
    UFUNCTION(BlueprintCallable, Category = "Attack System")
    void SetGlobalDamageMultiplier(float Multiplier) { GlobalDamageMultiplier = Multiplier; }

    UFUNCTION(BlueprintPure, Category = "Attack System")
    float GetGlobalDamageMultiplier() const { return GlobalDamageMultiplier; }

    // 事件
    UPROPERTY(BlueprintAssignable, Category = "Attack System")
    FOnAttackHit OnAttackHit;

    UPROPERTY(BlueprintAssignable, Category = "Attack System")
    FOnAttackStarted OnAttackStarted;

    UPROPERTY(BlueprintAssignable, Category = "Attack System")
    FOnAttackEnd OnAttackEnd;

protected:
    virtual void OnInitialize() override;
    virtual void OnReset() override;
    virtual void OnShutdown() override;

    // 内部处理函数
    bool ApplyDamageToTarget(AActor* Target, float Damage, AActor* Instigator);
    void BroadcastAttackHit(AActor* Attacker, AActor* Target, float Damage);

private:
    // 攻击配置
    UPROPERTY(EditAnywhere, Category = "Attack System")
    float GlobalDamageMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Attack System")
    float AttackCooldown = 0.5f;

    UPROPERTY(EditAnywhere, Category = "Attack System")
    bool bEnableDebugLogging = true;

    // 运行时数据
    UPROPERTY(VisibleAnywhere, Category = "Attack System")
    TSet<TWeakObjectPtr<AActor>> ActiveAttacks;

    UPROPERTY(VisibleAnywhere, Category = "Attack System")
    TMap<TWeakObjectPtr<AActor>, float> LastAttackTimes;

    // 统计数据
    UPROPERTY(VisibleAnywhere, Category = "Attack System")
    int32 TotalAttacksProcessed = 0;

    UPROPERTY(VisibleAnywhere, Category = "Attack System")
    int32 TotalDamageDealt = 0;
};