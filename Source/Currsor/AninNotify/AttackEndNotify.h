// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimNotify.h"
#include "AttackEndNotify.generated.h"

/**
 * 
 */
UCLASS()
class CURRSOR_API UAttackEndNotify : public UPaperZDAnimNotify
{
	GENERATED_BODY()
public:
	virtual FName GetDisplayName_Implementation() const override { return TEXT("AttackEnd"); }

	virtual void OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const override;
};
