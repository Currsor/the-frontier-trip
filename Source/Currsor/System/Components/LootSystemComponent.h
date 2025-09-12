// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSystemComponent.h"
#include "Engine/DataTable.h"
#include "LootSystemComponent.generated.h"

USTRUCT(BlueprintType)
struct FLootItem : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    FString ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    int32 ItemID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    float DropChance = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    int32 MinQuantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    int32 MaxQuantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    FString Rarity = TEXT("Common");

    FLootItem()
    {
        ItemName = TEXT("Unknown Item");
        ItemID = 0;
        DropChance = 0.1f;
        MinQuantity = 1;
        MaxQuantity = 1;
        Rarity = TEXT("Common");
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLootGenerated, AActor*, Source, const TArray<FLootItem>&, Items, FVector, Location);

/**
 * 掉落系统组件
 * 处理物品掉落逻辑和掉落表管理
 * 对应TypeScript中的LootSystem
 */
UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CURRSOR_API ULootSystemComponent : public UBaseSystemComponent
{
    GENERATED_BODY()

public:
    ULootSystemComponent();

    // 掉落处理
    UFUNCTION(BlueprintCallable, Category = "Loot System")
    TArray<FLootItem> GenerateLoot(AActor* Source, const FString& LootTableName = TEXT("Default"));

    UFUNCTION(BlueprintCallable, Category = "Loot System")
    void SpawnLoot(AActor* Source, const TArray<FLootItem>& Items, FVector Location);

    UFUNCTION(BlueprintCallable, Category = "Loot System")
    bool AddLootTable(const FString& TableName, const TArray<FLootItem>& Items);

    UFUNCTION(BlueprintCallable, Category = "Loot System")
    void ClearDropHistory();

    // 配置
    UFUNCTION(BlueprintCallable, Category = "Loot System")
    void SetGlobalDropRateMultiplier(float Multiplier) { GlobalDropRateMultiplier = Multiplier; }

    UFUNCTION(BlueprintPure, Category = "Loot System")
    float GetGlobalDropRateMultiplier() const { return GlobalDropRateMultiplier; }

    // 事件
    UPROPERTY(BlueprintAssignable, Category = "Loot System")
    FOnLootGenerated OnLootGenerated;

protected:
    virtual void OnInitialize() override;
    virtual void OnReset() override;
    virtual void OnShutdown() override;

private:
    // 内部函数
    bool RollForDrop(float DropChance) const;
    int32 RollQuantity(int32 MinQuantity, int32 MaxQuantity) const;

    // 掉落表 - 使用扁平化存储避免嵌套TArray问题
    UPROPERTY(EditAnywhere, Category = "Loot System")
    TMap<FString, int32> LootTableIndices;
    
    // 存储所有掉落物品的扁平化数组
    UPROPERTY(EditAnywhere, Category = "Loot System")
    TArray<FLootItem> AllLootItems;
    
    // 存储每个掉落表的起始索引和长度
    UPROPERTY(EditAnywhere, Category = "Loot System")
    TArray<int32> LootTableStartIndices;
    
    UPROPERTY(EditAnywhere, Category = "Loot System")
    TArray<int32> LootTableLengths;

    // 配置
    UPROPERTY(EditAnywhere, Category = "Loot System")
    float GlobalDropRateMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Loot System")
    bool bEnableDebugLogging = true;

    // 统计数据
    UPROPERTY(VisibleAnywhere, Category = "Loot System")
    int32 TotalDropsGenerated = 0;

    UPROPERTY(VisibleAnywhere, Category = "Loot System")
    TArray<FString> RecentDropHistory;
};