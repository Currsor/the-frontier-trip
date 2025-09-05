// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"

#include "Currsor/Character/Component/BaseState.h"


// Sets default values
ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	CurrentState = CreateDefaultSubobject<ABaseState>(TEXT("CurrentState"));
	CurrentState->SetHealth(100.0f);
}

void ABaseEnemy::ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator, const FHitResult& HitResult)
{
	IDamageable::ApplyDamage_Implementation(DamageAmount, DamageInstigator, HitResult);

	// 扣除血量
	CurrentState->SetHealth(CurrentState->GetHealth() - DamageAmount);
	
	if (CurrentState->GetHealth() <= 0.0f)
	{
		// TODO:死亡逻辑
		UE_LOG(LogTemp, Warning, TEXT("Enemy Die"));
	}
	else
	{
		// TODO:受击逻辑
		UE_LOG(LogTemp, Warning, TEXT("Enemy Take Damage: %f"), DamageAmount);
	}
}
