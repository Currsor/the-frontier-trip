// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimNotify.h"
#include "katana_AttackHitboxOff.generated.h"

/**
 * 
 */
UCLASS()
class CURRSOR_API Ukatana_AttackHitboxOff : public UPaperZDAnimNotify
{
	GENERATED_BODY()
public:
	virtual FName GetDisplayName_Implementation() const override { return TEXT("katana_AttackHitboxOff"); }

	virtual void OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const override;
};
