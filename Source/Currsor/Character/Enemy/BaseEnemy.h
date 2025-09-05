// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Currsor/Interface/IDamageable.h"
#include "BaseEnemy.generated.h"

class ABaseState;

UCLASS()
class CURRSOR_API ABaseEnemy : public APaperZDCharacter, public IDamageable
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	//~ Begin IDamageable Interface
	virtual void ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator, const FHitResult& HitResult) override;
	//~ End IDamageable Interface

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ABaseState> CurrentState;
};
