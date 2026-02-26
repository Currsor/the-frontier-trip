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

/** 敌人攻击方式枚举，可在蓝图中选择 */
UENUM(BlueprintType)
enum class EEnemyAttackType : uint8
{
	/** 普通攻击：对玩家造成固定伤害 */
	NormalAttack		UMETA(DisplayName = "普通攻击"),
	/** 强力攻击：造成更高伤害，但有前摇 */
	HeavyAttack			UMETA(DisplayName = "强力攻击"),
	/** 防御：本回合减少受到的伤害 */
	Defend				UMETA(DisplayName = "防御"),
	/** 技能攻击：使用特殊技能，例如施加状态效果 */
	SkillAttack			UMETA(DisplayName = "技能攻击"),
};

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

	// 获取敌人总数
	UFUNCTION(BlueprintPure, Category = "Enemy|Combat")
	int32 GetTotalEnemyCount() const { return TotalEnemyCount; }

	// 获取攻击方式
	UFUNCTION(BlueprintPure, Category = "Enemy|Combat")
	EEnemyAttackType GetAttackType() const { return AttackType; }

	// 获取攻击伤害值
	UFUNCTION(BlueprintPure, Category = "Enemy|Combat")
	float GetAttackDamage() const { return AttackDamage; }

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

	// 敌人总数，表示该敌人代表的敌人数量（默认为1，即单个敌人）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Combat", meta = (AllowPrivateAccess = "true"))
	int32 TotalEnemyCount = 1;

	// 攻击方式，可在蓝图细节面板中选择
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Combat", meta = (AllowPrivateAccess = "true"))
	EEnemyAttackType AttackType = EEnemyAttackType::NormalAttack;

	// 攻击伤害值，根据攻击方式不同可配置不同数值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Combat", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float AttackDamage = 10.0f;

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