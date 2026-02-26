// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth, float, DamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, DeadActor);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CURRSOR_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

    // 生命值管理
    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetMaxHealth(float NewMaxHealth);

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetCurrentHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealthPercentage() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsDead() const { return CurrentHealth <= 0.0f; }

    // 防御值管理（护盾机制：受到伤害时优先扣除防御值）
    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetDefense(float NewDefense);

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetDefense() const { return Defense; }

    UFUNCTION(BlueprintPure, Category = "Health")
    float CalculateDamageReduction(float IncomingDamage) const;

    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsFullHealth() const { return CurrentHealth >= MaxHealth; }

    // 伤害处理
    UFUNCTION(BlueprintCallable, Category = "Health")
    void TakeDamage(float DamageAmount, AActor* DamageInstigator = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void Heal(float HealAmount);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetCurrentHealth(float NewHealth);

    // Mana
    UFUNCTION(BlueprintCallable, Category = "Mana")
    int32 GetMaxManaCount() const { return MaxManaCount; }
    
    UFUNCTION(BlueprintCallable, Category = "Mana")
    int32 GetManaCount() const { return ManaCount; }

    UFUNCTION(BlueprintCallable, Category = "Mana")
    void UseMana(int32 ManaCost);

    // 事件委托
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnDeath OnDeath;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "1.0"))
    float MaxHealth = 40.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0.0"))
    float Defense = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    bool bCanTakeDamage = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    bool bAutoRespawn = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float RespawnDelay = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mana")
    int32 MaxManaCount = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mana")
    int32 ManaCount = MaxManaCount;

private:
    void HandleDeath();
    void BroadcastHealthChanged(float DamageAmount = 0.0f);
};