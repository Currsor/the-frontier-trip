#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ICombatInterface.generated.h"

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

	UFUNCTION(BlueprintNativeEvent, Category = "Combat|Hitbox")
	void AttackHitboxOn();

	UFUNCTION(BlueprintNativeEvent, Category = "Combat|Hitbox")
	void AttackHitboxOff();

};