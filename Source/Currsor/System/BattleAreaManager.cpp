// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleAreaManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Currsor/Character/Enemy/BaseEnemy.h"
#include "Currsor/System/Area/AreaCollisionBox.h"

void UBattleAreaManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UE_LOG(LogTemp, Log, TEXT("BattleAreaManager 已初始化"));
}

void UBattleAreaManager::SetEnemyAreaID(ABaseEnemy* Enemy, int32 AreaID)
{
	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetEnemyAreaID: 敌人为空"));
		return;
	}

	// 清理旧的映射关系
	if (EnemyAreaMap.Contains(Enemy))
	{
		int32 OldAreaID = EnemyAreaMap[Enemy];
		if (AreaEnemyMap.Contains(OldAreaID))
		{
			AreaEnemyMap[OldAreaID].Remove(Enemy);
			if (AreaEnemyMap[OldAreaID].Num() == 0)
			{
				AreaEnemyMap.Remove(OldAreaID);
			}
		}
	}

	// 设置新的映射关系
	EnemyAreaMap.Add(Enemy, AreaID);
	Enemy->SetAreaID(AreaID);

	// 更新区域到敌人的映射
	if (!AreaEnemyMap.Contains(AreaID))
	{
		AreaEnemyMap.Add(AreaID, TArray<TWeakObjectPtr<ABaseEnemy>>());
	}
	AreaEnemyMap[AreaID].AddUnique(Enemy);

	UE_LOG(LogTemp, Log, TEXT("敌人 %s 已分配到区域ID: %d"), *Enemy->GetName(), AreaID);
}

int32 UBattleAreaManager::GetEnemyAreaID(ABaseEnemy* Enemy) const
{
	if (!Enemy)
	{
		return -1;
	}

	if (const int32* AreaID = EnemyAreaMap.Find(Enemy))
	{
		return *AreaID;
	}

	return Enemy->GetAreaID();
}

void UBattleAreaManager::AssignEnemiesInArea(AAreaCollisionBox* AreaBox, float SearchRadius)
{
	if (!AreaBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("AssignEnemiesInArea: AreaBox 为空"));
		return;
	}

	FVector AreaCenter = AreaBox->GetActorLocation();
	TArray<ABaseEnemy*> EnemiesInRadius = FindEnemiesInRadius(AreaCenter, SearchRadius);

	// 获取区域ID（假设AreaBox有GetAreaID方法或者通过其他方式获取）
	int32 AreaID = GetTypeHash(AreaBox); // 临时使用哈希值作为ID

	for (ABaseEnemy* Enemy : EnemiesInRadius)
	{
		SetEnemyAreaID(Enemy, AreaID);
	}

	UE_LOG(LogTemp, Log, TEXT("已将 %d 个敌人分配到区域ID: %d"), EnemiesInRadius.Num(), AreaID);
}

void UBattleAreaManager::ClearEnemyAreaID(ABaseEnemy* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	if (EnemyAreaMap.Contains(Enemy))
	{
		int32 AreaID = EnemyAreaMap[Enemy];
		EnemyAreaMap.Remove(Enemy);

		// 从区域映射中移除
		if (AreaEnemyMap.Contains(AreaID))
		{
			AreaEnemyMap[AreaID].Remove(Enemy);
			if (AreaEnemyMap[AreaID].Num() == 0)
			{
				AreaEnemyMap.Remove(AreaID);
			}
		}
	}

	Enemy->SetAreaID(-1);
	UE_LOG(LogTemp, Log, TEXT("已清除敌人 %s 的区域ID"), *Enemy->GetName());
}

TArray<ABaseEnemy*> UBattleAreaManager::GetEnemiesInArea(int32 AreaID) const
{
	TArray<ABaseEnemy*> Result;

	if (const TArray<TWeakObjectPtr<ABaseEnemy>>* EnemyArray = AreaEnemyMap.Find(AreaID))
	{
		for (const TWeakObjectPtr<ABaseEnemy>& EnemyPtr : *EnemyArray)
		{
			if (EnemyPtr.IsValid())
			{
				Result.Add(EnemyPtr.Get());
			}
		}
	}

	return Result;
}

bool UBattleAreaManager::DoesEnemyHaveAreaID(ABaseEnemy* Enemy) const
{
	if (!Enemy)
	{
		return false;
	}

	return Enemy->HasAreaID() || EnemyAreaMap.Contains(Enemy);
}

TArray<ABaseEnemy*> UBattleAreaManager::FindEnemiesInRadius(const FVector& CenterLocation, float SearchRadius) const
{
	TArray<ABaseEnemy*> Result;
	
	UWorld* World = GetWorld();
	if (!World)
	{
		return Result;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ABaseEnemy::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(Actor))
		{
			float Distance = FVector::Dist(Enemy->GetActorLocation(), CenterLocation);
			if (Distance <= SearchRadius)
			{
				Result.Add(Enemy);
			}
		}
	}

	return Result;
}

void UBattleAreaManager::CleanupInvalidReferences()
{
	// 清理敌人到区域的映射中的无效引用
	TArray<TWeakObjectPtr<ABaseEnemy>> InvalidEnemies;
	for (auto& Pair : EnemyAreaMap)
	{
		if (!Pair.Key.IsValid())
		{
			InvalidEnemies.Add(Pair.Key);
		}
	}

	for (const TWeakObjectPtr<ABaseEnemy>& InvalidEnemy : InvalidEnemies)
	{
		EnemyAreaMap.Remove(InvalidEnemy);
	}

	// 清理区域到敌人的映射中的无效引用
	for (auto& Pair : AreaEnemyMap)
	{
		Pair.Value.RemoveAll([](const TWeakObjectPtr<ABaseEnemy>& EnemyPtr)
		{
			return !EnemyPtr.IsValid();
		});

		if (Pair.Value.Num() == 0)
		{
			AreaEnemyMap.Remove(Pair.Key);
		}
	}
}