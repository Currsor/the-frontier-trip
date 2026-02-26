// Copyright Dmitrii Labadin

#include "LogViewerBPLibraryPro.h"
#include "Blueprint/BlueprintSupport.h"
#include "BPLogCategoriesPro.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/World.h"


ULogViewerBPLibraryPro::ULogViewerBPLibraryPro(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


void ULogViewerBPLibraryPro::LogBP_Pro(const UObject* WorldContextObject, FName LogCategory, ELVPLogVerbosity Verbosity /*= ELVPLogVerbosity::EBPVerbosity_Log*/, const FString& InString /*= FString(TEXT("Hello"))*/, bool bUseNoCrashPrefix /* = true */)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) || USE_LOGGING_IN_SHIPPING // Do not Print in Shipping or Test unless explictly enabled.

	if (!WorldContextObject)
	{
		return;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return;
	}

	FString Prefix;
	if (World)
	{
		if (World->WorldType == EWorldType::PIE)
		{
			switch (World->GetNetMode())
			{
			case NM_Client:
				// GPlayInEditorID 0 is always the server, so 1 will be first client.
				// You want to keep this logic in sync with GeneratePIEViewportWindowTitle and UpdatePlayInEditorWorldDebugString
				Prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID);
				break;
			case NM_DedicatedServer:
			case NM_ListenServer:
				Prefix = FString::Printf(TEXT("Server: "));
				break;
			case NM_Standalone:
				break;
			}
		}
	}

	const FString FinalDisplayString = Prefix + InString;
	FString FinalLogString = FinalDisplayString;

	static const FBoolConfigValueHelper DisplayPrintStringSource(TEXT("Kismet"), TEXT("bLogPrintStringSource"), GEngineIni);
	if (DisplayPrintStringSource)
	{
		const FString SourceObjectPrefix = FString::Printf(TEXT("[%s] "), *GetNameSafe(WorldContextObject));
		FinalLogString = SourceObjectPrefix + FinalLogString;
	}

	if (bUseNoCrashPrefix)  //Prefix is required so user don't crash when accidentally defined existing C++ category. However you can disable that option to not lose performance on string concatenation
	{
		const FString LogCategoryWithPrefix = TEXT("LVP_") + LogCategory.ToString();
		const FName LogCategoryNameWithPrefix = FName(LogCategoryWithPrefix);
		const FLogCategory RealLogCategory = FLogCategory<ELogVerbosity::Type::Log, ELogVerbosity::Type::Log>(LogCategoryNameWithPrefix);
		LOG_BP_SELECT_VERBOSITY(RealLogCategory, Verbosity, InString);
	}
	else
	{
		const FLogCategory RealLogCategory = FLogCategory<ELogVerbosity::Type::Log, ELogVerbosity::Type::Log>(LogCategory);
		LOG_BP_SELECT_VERBOSITY(RealLogCategory, Verbosity, InString);
	}
#endif
}


