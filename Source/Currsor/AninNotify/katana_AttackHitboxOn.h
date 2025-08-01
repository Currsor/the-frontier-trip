// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimNotify.h"
#include "katana_AttackHitboxOn.generated.h"

/**
 * 
 */
UCLASS()
class CURRSOR_API Ukatana_AttackHitboxOn : public UPaperZDAnimNotify
{
	GENERATED_BODY()
public:
	virtual FName GetDisplayName_Implementation() const override { return TEXT("katana_AttackHitboxOn"); }

	virtual void OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const override;
};
