// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackEndNotify.h"

#include "PaperZDCharacter.h"
#include "PaperZDAnimInstance.h"
#include "Currsor/Interface/CombatInterface.h"

void UAttackEndNotify::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	Super::OnReceiveNotify_Implementation(OwningInstance);

	if (!OwningInstance) return;

	TObjectPtr<APaperZDCharacter> PaperChar = OwningInstance->GetPaperCharacter();

	if (PaperChar && PaperChar->GetController()->Implements<UCombatInterface>()) 
	{
		// 自动处理C++和蓝图的调用
		ICombatInterface::Execute_AttackEnd(PaperChar->GetController());
	}
}
