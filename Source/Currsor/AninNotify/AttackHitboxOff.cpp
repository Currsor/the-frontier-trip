// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackHitboxOff.h"

#include "PaperZDAnimInstance.h"
#include "PaperZDCharacter.h"
#include "Currsor/Interface/ICombatInterface.h"

void UAttackHitboxOff::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	Super::OnReceiveNotify_Implementation(OwningInstance);

	if (!OwningInstance) return;

	TObjectPtr<APaperZDCharacter> PaperChar = OwningInstance->GetPaperCharacter();

	if (PaperChar && PaperChar->GetController()->Implements<UCombatInterface>()) 
	{
		// 自动处理C++和蓝图的调用
		ICombatInterface::Execute_AttackHitboxOff(PaperChar->GetController());
	}
}
