#pragma once

#include "JsEnv.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CurrsorGameInstance.generated.h"

/**
 * 游戏实例类，管理游戏实例的初始化、运行和结束
 */
UCLASS()
class CURRSOR_API UCurrsorGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;

	virtual void OnStart() override;

	virtual void Shutdown() override;

	// PureTS调试模式
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug | PuerTS")
	bool bDebugMode = false;

	// 等待调试器
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug | PuerTS", meta = (EditCondition = "bDebugMode",EditConditionHides = "bDebugMode"))
	bool bWaitForDebugger = false;

	// 是否是调试模式
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug | Editor")  
	bool bDebug = false;

	// 是否是攻击调试模式
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug | Editor", meta = (EditCondition = "bDebug",EditConditionHides = "bDebug"))
	bool bAttackDebug = false;

private:
	TSharedPtr<puerts::FJsEnv> GameScript;
};
