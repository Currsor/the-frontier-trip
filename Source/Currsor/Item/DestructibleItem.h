// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Currsor/Interface/IDamageable.h"
#include "DestructibleItem.generated.h"

class UHealthComponent;
class UParticleSystem;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemDestroyed, ADestructibleItem*, DestroyedItem);

UCLASS(BlueprintType, Blueprintable)
class CURRSOR_API ADestructibleItem : public AStaticMeshActor, public IDamageable
{
    GENERATED_BODY()

public:
    ADestructibleItem();

    //~ Begin IDamageable Interface
    virtual void ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator, const FHitResult& HitResult) override;
    //~ End IDamageable Interface

    // 获取生命值组件
    UFUNCTION(BlueprintPure, Category = "Destructible")
    UHealthComponent* GetHealthComponent() const { return HealthComponent; }

    // 检查是否已被破坏
    UFUNCTION(BlueprintPure, Category = "Destructible")
    bool IsDestroyed() const { return bIsDestroyed; }

    // 手动触发破坏
    UFUNCTION(BlueprintCallable, Category = "Destructible")
    void DestroyItem();

    // 事件委托
    UPROPERTY(BlueprintAssignable, Category = "Destructible")
    FOnItemDestroyed OnItemDestroyed;

protected:
    virtual void BeginPlay() override;

    // 组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UHealthComponent* HealthComponent;

    // 破坏状态
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Destructible")
    bool bIsDestroyed = false;

    // 破坏效果
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UParticleSystem* DestructionEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    USoundBase* DestructionSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    FVector EffectScale = FVector(1.0f);

    // 掉落物品
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    TArray<TSubclassOf<AActor>> DropItemClasses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    int32 MinDropCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    int32 MaxDropCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    float DropRadius = 100.0f;

    // 破坏行为
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible")
    bool bDestroyActorOnDeath = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible", meta = (EditCondition = "bDestroyActorOnDeath"))
    float DestroyDelay = 2.0f;

    // 事件函数
    UFUNCTION()
    void OnHealthDepleted(AActor* DeadActor);

    UFUNCTION(BlueprintImplementableEvent, Category = "Destructible")
    void OnItemDestroyedBP();

    UFUNCTION(BlueprintNativeEvent, Category = "Destructible")
    void HandleDestruction();
    virtual void HandleDestruction_Implementation();

    UFUNCTION(BlueprintNativeEvent, Category = "Loot")
    void SpawnLoot();
    virtual void SpawnLoot_Implementation();

    UFUNCTION(BlueprintNativeEvent, Category = "Effects")
    void PlayDestructionEffects();
    virtual void PlayDestructionEffects_Implementation();
};