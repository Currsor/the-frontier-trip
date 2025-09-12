// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Currsor/Interface/IDamageable.h"
#include "BaseEnemy.generated.h"

class ABaseState;
class UHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, ABaseEnemy*, DeadEnemy);

UCLASS()
class CURRSOR_API ABaseEnemy : public APaperZDCharacter, public IDamageable
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	//~ Begin IDamageable Interface
	virtual void ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator, const FHitResult& HitResult) override;
	//~ End IDamageable Interface

	// 获取生命值组件
	UFUNCTION(BlueprintPure, Category = "Enemy")
	UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	// 获取状态组件
	UFUNCTION(BlueprintPure, Category = "Enemy")
	ABaseState* GetStateComponent() const { return CurrentState; }

	// 检查是否死亡
	UFUNCTION(BlueprintPure, Category = "Enemy")
	bool IsDead() const;

	// 事件委托
	UPROPERTY(BlueprintAssignable, Category = "Enemy")
	FOnEnemyDeath OnEnemyDeath;

protected:
	virtual void BeginPlay() override;

	// 组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	ABaseState* CurrentState;

	// 死亡处理
	UFUNCTION()
	void OnHealthDepleted(AActor* DeadActor);

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void HandleDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnDeathBP();

	// 受伤处理
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnTakeDamageBP(float DamageAmount, AActor* DamageInstigator);
};
