#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class ICombatInterface
{
	GENERATED_BODY()

public:
	// 基础战斗状态
	UFUNCTION(BlueprintNativeEvent, Category = "Combat|State")
	void AttackEnd();

};