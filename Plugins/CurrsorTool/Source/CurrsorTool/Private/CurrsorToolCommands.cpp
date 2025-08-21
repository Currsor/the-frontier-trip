// Copyright Epic Games, Inc. All Rights Reserved.

#include "CurrsorToolCommands.h"

#define LOCTEXT_NAMESPACE "FCurrsorToolModule"

void FCurrsorToolCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "CurrsorTool", "Bring up CurrsorTool window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
