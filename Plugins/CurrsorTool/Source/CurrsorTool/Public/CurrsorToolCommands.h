// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "CurrsorToolStyle.h"

class FCurrsorToolCommands : public TCommands<FCurrsorToolCommands>
{
public:

	FCurrsorToolCommands()
		: TCommands<FCurrsorToolCommands>(TEXT("CurrsorTool"), NSLOCTEXT("Contexts", "CurrsorTool", "CurrsorTool Plugin"), NAME_None, FCurrsorToolStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};