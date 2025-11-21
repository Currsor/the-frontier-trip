#include "CurrsorGameInstance.h"

void UCurrsorGameInstance::InitializePuerTS()
{
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

void UCurrsorGameInstance::Init()
{
	Super::Init();
	
	// 在打包版本中初始化 PuerTS
#if !WITH_EDITOR
	InitializePuerTS();
#endif
}

void UCurrsorGameInstance::OnStart()
{
	Super::OnStart();
	
	// 在编辑器中初始化 PuerTS
#if WITH_EDITOR
	InitializePuerTS();
#endif
}

void UCurrsorGameInstance::Shutdown()
{
	Super::Shutdown();
	GameScript.Reset();
}
