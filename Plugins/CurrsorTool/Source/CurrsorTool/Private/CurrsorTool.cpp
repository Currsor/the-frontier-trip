// Copyright Epic Games, Inc. All Rights Reserved.

#include "CurrsorTool.h"
#include "CurrsorToolStyle.h"
#include "CurrsorToolCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName CurrsorToolTabName("CurrsorTool");

#define LOCTEXT_NAMESPACE "FCurrsorToolModule"

void FCurrsorToolModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FCurrsorToolStyle::Initialize();
	FCurrsorToolStyle::ReloadTextures();

	FCurrsorToolCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FCurrsorToolCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FCurrsorToolModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCurrsorToolModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(CurrsorToolTabName, FOnSpawnTab::CreateRaw(this, &FCurrsorToolModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FCurrsorToolTabTitle", "CurrsorTool"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FCurrsorToolModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FCurrsorToolStyle::Shutdown();

	FCurrsorToolCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(CurrsorToolTabName);
}

TSharedRef<SDockTab> FCurrsorToolModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FCurrsorToolModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("CurrsorTool.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FCurrsorToolModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(CurrsorToolTabName);
}

void FCurrsorToolModule::CurrsorToolMenu(UToolMenu* InMenu)
{
	FToolMenuSection& Section = InMenu->AddSection("Section");
	const FCurrsorToolCommands& Commands = FCurrsorToolCommands::Get();
	
	Section.AddSubMenu(
		"TestSubMenu",
		LOCTEXT("TestSubMenu", "Test SubMenu"),
		LOCTEXT("TestSubMenu_ToolTip", "Test SubMenu Tooltip"),
		FNewToolMenuDelegate::CreateRaw(this, &FCurrsorToolModule::FillExportAssetMenu)
		);
}

void FCurrsorToolModule::FillExportAssetMenu(UToolMenu* InMenu)
{
	FToolMenuSection& Section = InMenu->AddSection("Section");
	const FCurrsorToolCommands& Commands = FCurrsorToolCommands::Get();
	Section.AddMenuEntryWithCommandList(Commands.OpenPluginWindow, PluginCommands);
}

void FCurrsorToolModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("Tools");
			Section.AddMenuEntryWithCommandList(FCurrsorToolCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
		if (ToolbarMenu == nullptr)
		{
			ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		}
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			
			FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitComboButton(
				"CurrsorTool",
				FUIAction(),
				FNewToolMenuDelegate::CreateRaw(this, &FCurrsorToolModule::CurrsorToolMenu),
				LOCTEXT("CurrsorTool", "Currsor Tool"),
				LOCTEXT("CurrsorToolTooltip", "Currsor Tool Tooltip"),
				FSlateIcon(FCurrsorToolStyle::GetStyleSetName(), "CurrsorTool.OpenPluginWindow")
				));
			Entry.SetCommandList(PluginCommands);
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCurrsorToolModule, CurrsorTool)