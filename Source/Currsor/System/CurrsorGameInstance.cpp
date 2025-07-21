#include "CurrsorGameInstance.h"

void UCurrsorGameInstance::Init()
{
	Super::Init();
}

void UCurrsorGameInstance::OnStart()
{
	Super::OnStart();
	if (bDebugMode)
	{
		GameScript = MakeShared<puerts::FJsEnv>(
			std::make_unique<puerts::DefaultJSModuleLoader>(TEXT("JavaScript")),
			std::make_shared<puerts::FDefaultLogger>(),
			8080
			);

		if (bWaitForDebugger)
		{
			GameScript->WaitDebugger();
		}
	}
	else
	{
		GameScript = MakeShared<puerts::FJsEnv>();
	}
	
	GameScript->Start("CurrsorGame");
}

void UCurrsorGameInstance::Shutdown()
{
	Super::Shutdown();
	GameScript.Reset();
}
