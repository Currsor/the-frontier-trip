// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Area/CurrsorAreaManager.h"
#include "GameFramework/GameState.h"
#include "CurrsorGameState.generated.h"

/**
 * 战斗状态
 */
UENUM()
enum class ECombatState : uint8 {
	Default,    // 非战斗状态
	Combat,     // 战斗中
	Victory,    // 战斗胜利
	Defeat      // 战斗失败
};

/**
 * 游戏状态类，管理游戏的各种状态
 */
UCLASS()
class CURRSOR_API ACurrsorGameState : public AGameState
{
	GENERATED_BODY()

public:
	// 构造函数
	ACurrsorGameState();

	// ========== 区域相关 ==========
private:
	UPROPERTY(VisibleAnywhere)
	int32 CurrentAreaID;

	UPROPERTY(EditDefaultsOnly, Category = "State")
	TObjectPtr<ACurrsorAreaManager> AreaManager = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE int32 GetCurrentAreaID() const { return CurrentAreaID; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetCurrentAreaID(int32 InCurrentAreaID) { CurrentAreaID = InCurrentAreaID; UE_LOG(LogTemp, Log, TEXT("设置当前区域ID: %d"), InCurrentAreaID); }
	
	TObjectPtr<AAreaCollisionBox> GetActorFromID(int32 InID) const;

	UFUNCTION(BlueprintCallable, Category = "State")
	FString GetNameFromID(int32 InID) const;

	UFUNCTION(BlueprintCallable)
	void SetAreaManager(ACurrsorAreaManager* InAreaManager);
	
	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE ACurrsorAreaManager* GetAreaManager() const { return AreaManager; }

	// ========== 战斗状态相关 ==========
	private:
	UPROPERTY(VisibleAnywhere)
	ECombatState CombatState;

public:
	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

	UFUNCTION(BlueprintCallable, Category = "State")
	void SetCombatState(ECombatState InCombatState);

	// ========== 玩家相关 ==========
	private:
	UPROPERTY(VisibleAnywhere)
	FVector LastPlayerCombatPosition;
	UPROPERTY(VisibleAnywhere)
	float CurrentPlayerHealth;
	UPROPERTY(VisibleAnywhere)
	int32 TotalPlayerCount;
	UPROPERTY(VisibleAnywhere)
	int32 PlayerDeathCount;

public:
	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE FVector GetLastPlayerCombatPosition() const { return LastPlayerCombatPosition; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetLastPlayerCombatPosition(FVector InLastPlayerCombatPosition) { LastPlayerCombatPosition = InLastPlayerCombatPosition; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE float GetCurrentPlayerHealth() const { return CurrentPlayerHealth; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetCurrentPlayerHealth(float InCurrentPlayerHealth) { CurrentPlayerHealth = InCurrentPlayerHealth; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE int32 GetTotalPlayerCount() const { return TotalPlayerCount; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetTotalPlayerCount(int32 InTotalPlayerCount) { TotalPlayerCount = InTotalPlayerCount; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE int32 GetPlayerDeathCount() const { return PlayerDeathCount; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetPlayerDeathCount(int32 InPlayerDeathCount) { PlayerDeathCount = InPlayerDeathCount; }

	UFUNCTION(BlueprintCallable, Category = "State")
	void IncrementPlayerDeathCount();

	UFUNCTION(BlueprintCallable, Category = "State")
	bool AreAllPlayersDead() const;

	// ========== 敌人相关 ==========
	private:
	UPROPERTY(VisibleAnywhere)
	FVector LastEnemyCombatPosition;
	UPROPERTY(VisibleAnywhere)
	float CurrentEnemyHealth;
	UPROPERTY(VisibleAnywhere)
	int32 TotalEnemyCount;
	UPROPERTY(VisibleAnywhere)
	int32 EnemyDeathCount;

public:
	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE FVector GetLastEnemyCombatPosition() const { return LastEnemyCombatPosition; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetLastEnemyCombatPosition(FVector InLastEnemyCombatPosition) { LastEnemyCombatPosition = InLastEnemyCombatPosition; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE float GetCurrentEnemyHealth() const { return CurrentEnemyHealth; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetCurrentEnemyHealth(float InCurrentEnemyHealth) { CurrentEnemyHealth = InCurrentEnemyHealth; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE int32 GetTotalEnemyCount() const { return TotalEnemyCount; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetTotalEnemyCount(int32 InTotalEnemyCount) { TotalEnemyCount = InTotalEnemyCount; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE int32 GetEnemyDeathCount() const { return EnemyDeathCount; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetEnemyDeathCount(int32 InEnemyDeathCount) { EnemyDeathCount = InEnemyDeathCount; }

	UFUNCTION(BlueprintCallable, Category = "State")
	void IncrementEnemyDeathCount();

	UFUNCTION(BlueprintCallable, Category = "State")
	bool AreAllEnemiesDead() const;

	UFUNCTION(BlueprintCallable, Category = "State")
	void BroadcastCombatStateChange(const FString& CombatEventType);
};