// Fill out your copyright notice in the Description page of Project Settings.

#include "LootSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"

ULootSystemComponent::ULootSystemComponent()
{
    SystemName = TEXT("LootSystem");
    PrimaryComponentTick.bCanEverTick = false;
}

void ULootSystemComponent::OnInitialize()
{
    Super::OnInitialize();
    
    // 初始化默认掉落表
    TArray<FLootItem> DefaultLoot;
    
    FLootItem CommonItem;
    CommonItem.ItemName = TEXT("Common Drop");
    CommonItem.ItemID = 1;
    CommonItem.DropChance = 0.5f;
    CommonItem.Rarity = TEXT("Common");
    DefaultLoot.Add(CommonItem);
    
    FLootItem RareItem;
    RareItem.ItemName = TEXT("Rare Drop");
    RareItem.ItemID = 2;
    RareItem.DropChance = 0.1f;
    RareItem.Rarity = TEXT("Rare");
    DefaultLoot.Add(RareItem);
    
    // 添加到扁平化数组
    int32 StartIndex = AllLootItems.Num();
    AllLootItems.Append(DefaultLoot);
    
    // 记录表信息
    int32 TableIndex = LootTableStartIndices.Num();
    LootTableStartIndices.Add(StartIndex);
    LootTableLengths.Add(DefaultLoot.Num());
    
    // 添加索引映射
    LootTableIndices.Add(TEXT("Default"), TableIndex);
    
    // 清理统计数据
    TotalDropsGenerated = 0;
    RecentDropHistory.Empty();

    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("LootSystem initialized with %d loot tables"), LootTableIndices.Num());
    }
}

void ULootSystemComponent::OnReset()
{
    Super::OnReset();
    
    // 重置统计数据
    TotalDropsGenerated = 0;
    RecentDropHistory.Empty();
    
    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("LootSystem reset"));
    }
}

void ULootSystemComponent::OnShutdown()
{
    Super::OnShutdown();
    
    // 清理掉落表
    LootTableIndices.Empty();
    AllLootItems.Empty();
    LootTableStartIndices.Empty();
    LootTableLengths.Empty();
    RecentDropHistory.Empty();
    
    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("LootSystem shutdown - Total drops generated: %d"), TotalDropsGenerated);
    }
}

TArray<FLootItem> ULootSystemComponent::GenerateLoot(AActor* Source, const FString& LootTableName)
{
    TArray<FLootItem> GeneratedLoot;
    
    if (!bIsInitialized)
    {
        UE_LOG(LogTemp, Error, TEXT("LootSystem not initialized"));
        return GeneratedLoot;
    }

    if (!Source)
    {
        UE_LOG(LogTemp, Error, TEXT("GenerateLoot: Invalid source actor"));
        return GeneratedLoot;
    }

    // 查找掉落表
    int32* TableIndexPtr = LootTableIndices.Find(LootTableName);
    if (!TableIndexPtr)
    {
        if (bEnableDebugLogging)
        {
            UE_LOG(LogTemp, Warning, TEXT("Loot table not found: %s, using Default"), *LootTableName);
        }
        TableIndexPtr = LootTableIndices.Find(TEXT("Default"));
    }

    if (!TableIndexPtr || !LootTableStartIndices.IsValidIndex(*TableIndexPtr) || !LootTableLengths.IsValidIndex(*TableIndexPtr))
    {
        UE_LOG(LogTemp, Error, TEXT("No loot tables available"));
        return GeneratedLoot;
    }
    
    // 获取表的范围
    int32 StartIndex = LootTableStartIndices[*TableIndexPtr];
    int32 Length = LootTableLengths[*TableIndexPtr];

    // 生成掉落
    for (int32 i = StartIndex; i < StartIndex + Length && i < AllLootItems.Num(); ++i)
    {
        const FLootItem& Item = AllLootItems[i];
        float AdjustedDropChance = Item.DropChance * GlobalDropRateMultiplier;
        
        if (RollForDrop(AdjustedDropChance))
        {
            FLootItem DroppedItem = Item;
            DroppedItem.MinQuantity = RollQuantity(Item.MinQuantity, Item.MaxQuantity);
            DroppedItem.MaxQuantity = DroppedItem.MinQuantity; // 设置为实际掉落数量
            
            GeneratedLoot.Add(DroppedItem);
            TotalDropsGenerated++;
            
            // 记录掉落历史
            FString DropRecord = FString::Printf(TEXT("%s x%d from %s"), 
                                               *Item.ItemName, 
                                               DroppedItem.MinQuantity, 
                                               *Source->GetName());
            RecentDropHistory.Add(DropRecord);
            
            // 限制历史记录数量
            if (RecentDropHistory.Num() > 50)
            {
                RecentDropHistory.RemoveAt(0);
            }
            
            if (bEnableDebugLogging)
            {
                UE_LOG(LogTemp, Log, TEXT("Loot generated: %s"), *DropRecord);
            }
        }
    }

    // 广播掉落生成事件
    if (GeneratedLoot.Num() > 0)
    {
        OnLootGenerated.Broadcast(Source, GeneratedLoot, Source->GetActorLocation());
    }

    return GeneratedLoot;
}

void ULootSystemComponent::SpawnLoot(AActor* Source, const TArray<FLootItem>& Items, FVector Location)
{
    if (!Source || Items.Num() == 0)
    {
        return;
    }

    // 这里可以实现实际的掉落物品生成逻辑
    // 例如生成掉落物品Actor、设置位置等
    
    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("Spawning %d loot items at location %s"), 
               Items.Num(), *Location.ToString());
    }
}

bool ULootSystemComponent::AddLootTable(const FString& TableName, const TArray<FLootItem>& Items)
{
    if (TableName.IsEmpty() || Items.Num() == 0)
    {
        return false;
    }

    // 添加到扁平化数组
    int32 StartIndex = AllLootItems.Num();
    AllLootItems.Append(Items);
    
    // 记录表信息
    int32 TableIndex = LootTableStartIndices.Num();
    LootTableStartIndices.Add(StartIndex);
    LootTableLengths.Add(Items.Num());
    
    // 添加索引映射
    LootTableIndices.Add(TableName, TableIndex);
    
    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("Added loot table '%s' with %d items"), *TableName, Items.Num());
    }
    
    return true;
}

void ULootSystemComponent::ClearDropHistory()
{
    RecentDropHistory.Empty();
    
    if (bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("Drop history cleared"));
    }
}

bool ULootSystemComponent::RollForDrop(float DropChance) const
{
    return FMath::RandRange(0.0f, 1.0f) <= FMath::Clamp(DropChance, 0.0f, 1.0f);
}

int32 ULootSystemComponent::RollQuantity(int32 MinQuantity, int32 MaxQuantity) const
{
    return FMath::RandRange(MinQuantity, MaxQuantity);
}