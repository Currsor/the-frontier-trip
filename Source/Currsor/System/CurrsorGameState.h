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
	FORCEINLINE void SetCurrentAreaID(int32 InCurrentAreaID) { CurrentAreaID = InCurrentAreaID; UE_LOG(LogTemp, Log, TEXT("SetCurrentAreaID: %d"), InCurrentAreaID); }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE AActor* GetActorFromID(int32 InID) const;

	UFUNCTION(BlueprintCallable)
	void SetAreaManager(ACurrsorAreaManager* InAreaManager) { AreaManager = InAreaManager; }

	// ========== 战斗状态相关 ==========
	private:
	UPROPERTY(VisibleAnywhere)
	ECombatState CombatState;

public:
	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetCombatState(ECombatState InCombatState) { CombatState = InCombatState; }

	// ========== 玩家相关 ==========
	private:
	UPROPERTY(VisibleAnywhere)
	FVector LastPlayerCombatPosition;
	UPROPERTY(VisibleAnywhere)
	float CurrentPlayerHealth;

public:
	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE FVector GetLastPlayerCombatPosition() const { return LastPlayerCombatPosition; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetLastPlayerCombatPosition(FVector InLastPlayerCombatPosition) { LastPlayerCombatPosition = InLastPlayerCombatPosition; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE float GetCurrentPlayerHealth() const { return CurrentPlayerHealth; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetCurrentPlayerHealth(float InCurrentPlayerHealth) { CurrentPlayerHealth = InCurrentPlayerHealth; }

	// ========== 敌人相关 ==========
	private:
	UPROPERTY(VisibleAnywhere)
	FVector LastEnemyCombatPosition;
	UPROPERTY(VisibleAnywhere)
	float CurrentEnemyHealth;

public:
	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE FVector GetLastEnemyCombatPosition() const { return LastEnemyCombatPosition; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetLastEnemyCombatPosition(FVector InLastEnemyCombatPosition) { LastEnemyCombatPosition = InLastEnemyCombatPosition; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE float GetCurrentEnemyHealth() const { return CurrentEnemyHealth; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FORCEINLINE void SetCurrentEnemyHealth(float InCurrentEnemyHealth) { CurrentEnemyHealth = InCurrentEnemyHealth; }
};