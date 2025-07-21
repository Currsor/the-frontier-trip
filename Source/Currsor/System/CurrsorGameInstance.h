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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDebugMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bWaitForDebugger = false;

private:
	TSharedPtr<puerts::FJsEnv> GameScript;
};
