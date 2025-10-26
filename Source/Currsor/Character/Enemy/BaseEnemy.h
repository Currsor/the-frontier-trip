// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Currsor/Interface/IDamageable.h"
#include "BaseEnemy.generated.h"

class ABaseState;
class UHealthComponent;
class AAreaCollisionBox;

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

	// 区域ID相关功能
	UFUNCTION(BlueprintPure, Category = "Enemy|Area")
	int32 GetAreaID() const { return AreaID; }

	UFUNCTION(BlueprintCallable, Category = "Enemy|Area")
	void SetAreaID(int32 InAreaID) { AreaID = InAreaID; }

	UFUNCTION(BlueprintPure, Category = "Enemy|Area")
	bool HasAreaID() const { return AreaID != -1; }

	// AreaCollisionBox选择相关功能
	UFUNCTION(BlueprintPure, Category = "Enemy|Area")
	AAreaCollisionBox* GetSelectedAreaBox() const { return SelectedAreaBox; }

	UFUNCTION(BlueprintCallable, Category = "Enemy|Area")
	void SetSelectedAreaBox(AAreaCollisionBox* InAreaBox) { SelectedAreaBox = InAreaBox; }

	// 从选中的AreaCollisionBox读取AreaID
	UFUNCTION(BlueprintCallable, Category = "Enemy|Area")
	void ReadAreaIDFromSelectedBox();

	// 检查是否有选中的区域盒子
	UFUNCTION(BlueprintPure, Category = "Enemy|Area")
	bool HasSelectedAreaBox() const { return SelectedAreaBox != nullptr; }

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

	// 可在蓝图编辑器中选择的区域碰撞盒
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Area", meta = (AllowPrivateAccess = "true"))
	AAreaCollisionBox* SelectedAreaBox = nullptr;

	// 区域ID，-1表示没有关联区域
	// 注意：这个值会在BeginPlay时从SelectedAreaBox自动读取
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Area", meta = (AllowPrivateAccess = "true"))
	int32 AreaID = -1;

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
