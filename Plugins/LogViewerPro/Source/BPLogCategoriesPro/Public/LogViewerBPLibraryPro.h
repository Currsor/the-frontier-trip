// Copyright Dmitrii Labadin

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Logging/LogCategory.h"

#include "LogViewerBPLibraryPro.generated.h"

UENUM(BlueprintType)
enum class ELVPLogVerbosity : uint8
{
	ELVPVerbosity_VeryVerbose UMETA(DisplayName = "VeryVerbose"),
	ELVPVerbosity_Verbose UMETA(DisplayName = "Verbose"),
	ELVPVerbosity_Log UMETA(DisplayName = "Log"),
	ELVPVerbosity_Display UMETA(DisplayName = "Display"),
	ELVPVerbosity_Warning UMETA(DisplayName = "Warning"),
	ELVPVerbosity_Error UMETA(DisplayName = "Error"),
	ELVPVerbosity_Fatal UMETA(DisplayName = "Fatal"),
};

#define LOG_BP_SELECT_VERBOSITY(LogCategory, Verbosity, LogMessage) \
    switch (Verbosity) \
    { \
    case ELVPLogVerbosity::ELVPVerbosity_VeryVerbose: \
        UE_LOG(LogCategory, VeryVerbose, TEXT("%s"), *LogMessage); \
        break; \
    case ELVPLogVerbosity::ELVPVerbosity_Verbose: \
        UE_LOG(LogCategory, Verbose, TEXT("%s"), *LogMessage); \
        break; \
    case ELVPLogVerbosity::ELVPVerbosity_Log: \
        UE_LOG(LogCategory, Log, TEXT("%s"), *LogMessage); \
        break; \
    case ELVPLogVerbosity::ELVPVerbosity_Display: \
        UE_LOG(LogCategory, Display, TEXT("%s"), *LogMessage); \
        break; \
    case ELVPLogVerbosity::ELVPVerbosity_Warning: \
        UE_LOG(LogCategory, Warning, TEXT("%s"), *LogMessage); \
        break; \
    case ELVPLogVerbosity::ELVPVerbosity_Error: \
        UE_LOG(LogCategory, Error, TEXT("%s"), *LogMessage); \
        break; \
    case ELVPLogVerbosity::ELVPVerbosity_Fatal: \
        UE_LOG(LogCategory, Fatal, TEXT("%s"), *LogMessage); \
        break; \
    default: \
        UE_LOG(LogCategory, Log, TEXT("Unknown Verbosity Level: %s"), *LogMessage); \
        break; \
    }

UCLASS()
class ULogViewerBPLibraryPro : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/*
	Log strings with user-defined category.
	IMPORTANT! Prefixed "LVP_" is added by default to prevent crash caused by collision with predefined engine categories!

	If you need extra performance from that node:
	To gain extra performance from not having string concatenation you can disable prefix logic by setting bUsePrefixToAvoidCrash to false.
	It will be your responsibility to make sure that you're not using existing log categories defined in the engine such as LogWorld, LogAudio, or others.
	If predefined engine category is used engine will crash with "Log suppression category NAME is defined multiple times with different compile time verbosity".
	*/
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "4", DevelopmentOnly), Category = "Development")
	static void LogBP_Pro(const UObject* WorldContextObject, FName LogCategory = FName("Temp"), ELVPLogVerbosity Verbosity = ELVPLogVerbosity::ELVPVerbosity_Log, const FString& InString = FString(TEXT("Hello")), bool bUsePrefixToAvoidCrash = true);

};
