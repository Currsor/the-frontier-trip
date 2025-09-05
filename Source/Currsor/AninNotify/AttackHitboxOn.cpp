// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackHitboxOn.h"

#include "PaperZDAnimInstance.h"
#include "PaperZDCharacter.h"
#include "Currsor/Interface/ICombatInterface.h"

void UAttackHitboxOn::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	Super::OnReceiveNotify_Implementation(OwningInstance);

	if (!OwningInstance) return;

	TObjectPtr<APaperZDCharacter> PaperChar = OwningInstance->GetPaperCharacter();

	if (PaperChar && PaperChar->GetController()->Implements<UCombatInterface>()) 
	{
		// 自动处理C++和蓝图的调用
		ICombatInterface::Execute_AttackHitboxOn(PaperChar->GetController());
	}
}
