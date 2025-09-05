// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimNotify.h"
#include "AttackHitboxOn.generated.h"

/**
 * 
 */
UCLASS()
class CURRSOR_API UAttackHitboxOn : public UPaperZDAnimNotify
{
	GENERATED_BODY()
public:
	virtual FName GetDisplayName_Implementation() const override { return TEXT("AttackHitboxOn"); }

	virtual void OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const override;
};
