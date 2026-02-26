// Copyright Dmitrii Labadin 2019

#include "LogViewerProFileMenu.h"
#include "LogViewerProWidgetCategoriesView.h"
#include "LogViewerProWidgetMain.h"
#include "SlateOptMacros.h"
#include "Styling/CoreStyle.h"

#include "Misc/FileHelper.h"
#include "DesktopPlatformModule.h"

#include "LogViewerProStructs.h"
#include "../Public/LogViewerProWidgetTopBar.h"

#define LOCTEXT_NAMESPACE "SLogViewerProWidgetMain"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLogViewerProFileMenu::Construct(const FArguments& InArgs)
{
	MainWidget = InArgs._MainWidget;

	SComboButton::Construct(SComboButton::FArguments()
		.ContentPadding(0)
		.ForegroundColor(FSlateColor::UseForeground())
		.ComboButtonStyle(FAppStyle::Get(), "SimpleComboButton")
		.HasDownArrow(true)
		.OnGetMenuContent(this, &SLogViewerProFileMenu::GenerateFileMenu)
		.ButtonContent()
		[
			SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.Padding(FMargin(4.0, 2.0))
				[
					SNew(STextBlock)
						.Text(LOCTEXT("File", "File"))
				]
		]
	);

	//FSlateColorBrush brushClr(FLinearColor::White);
}

void SLogViewerProFileMenu::OpenLog()
{
	MainWidget->TopBar->OpenLog();
}

void SLogViewerProFileMenu::SaveLog()
{
	MainWidget->TopBar->SaveLog();
}

//SETTINGS
TSharedRef<SWidget> SLogViewerProFileMenu::GenerateFileMenu()
{
	// Get all menu extenders for this context menu from the content browser module

	FMenuBuilder MenuBuilder(/*bInShouldCloseWindowAfterMenuSelection=*/true, nullptr, nullptr, /*bCloseSelfOnly=*/ true);

	//MenuBuilder.AddMenuEntry(LOCTEXT("CollapseAll", "Collapse All"), LOCTEXT("CollapseAll_Tooltip", "Collapses the entire tree"), FSlateIcon(), FUIAction(FExecuteAction::CreateSP(this, &SClassViewer::SetAllExpansionStates, bool(false))), NAME_None, EUserInterfaceActionType::Button);

	MenuBuilder.BeginSection("Log Files", LOCTEXT("LVSettingsLog Files", "Log Files"));
	{
		MenuBuilder.AddMenuEntry(LOCTEXT("LVOpenLog", "Open Log"), LOCTEXT("LVOpenLogTip", "Open Log file from your file system"), FSlateIcon(), FUIAction(FExecuteAction::CreateSP(this, &SLogViewerProFileMenu::OpenLog)), NAME_None, EUserInterfaceActionType::Button);
		MenuBuilder.AddMenuEntry(LOCTEXT("LVOpenLog", "Save Log"), LOCTEXT("LVOpenLogTip", "Save Log file from your file system"), FSlateIcon(), FUIAction(FExecuteAction::CreateSP(this, &SLogViewerProFileMenu::SaveLog)), NAME_None, EUserInterfaceActionType::Button);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("Named Tabs", LOCTEXT("Named Tabs", "Named Tabs"));
	{
		MenuBuilder.AddMenuEntry(LOCTEXT("LogViewerSettingsVisualSettings_SaveNamedsTab", "Save as Named Tab"), LOCTEXT("LogViewerSettingsVisualSettings_SaveNamedsTabTooltip", "Give this tab a name to keep it between engine restarts"), FSlateIcon(), FUIAction(FExecuteAction::CreateSP(this, &SLogViewerProFileMenu::OpenPopup_SaveNamedTab)), NAME_None, EUserInterfaceActionType::Button);
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

void SLogViewerProFileMenu::OpenPopup_SaveNamedTab()
{

	SAssignNew(SaveNamedTabWindow, SWindow)
		.Title(LOCTEXT("SavenamedTabText", "Save Named Tab"))
		.ClientSize(FVector2D(500, 175))
		.IsPopupWindow(false);

	TSharedPtr<SBox> SaveNamedTabContent = SNew(SBox)
		.Padding(10.0f)
		[
			SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.Padding(4)
				.AutoHeight()
				[
					SNew(STextBlock)
						.Text(LOCTEXT("SavenamedTabHint", "Save Named Tab: \r\n Named tab stays open between engine restarts until closed. \r\n Named tab preserves its categories (to preserve logs, use Save Log). \r\n You can have multiple named tabs with different active categories. \r\n Named tabs stored in %PROJECT%/Config/LogViewerPro/NamedTabs/ "))
				]

				+ SVerticalBox::Slot()
				.Padding(4)
				.AutoHeight()
				[
					SAssignNew(TabNameTextBox, SEditableTextBox)
						.HintText(LOCTEXT("EditBoxTabNameHint", "Enter Custom Tab Name"))
						//.OnTextCommitted(this, &SLogViewerProWidgetTopBar::OnHighlightTextCommitted)
						//.OnTextChanged(this, &SLogViewerProWidgetTopBar::OnHighlightTextChanged)
						//.DelayChangeNotificationsWhileTyping(true)
				]
				+ SVerticalBox::Slot()
				.Padding(4)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.Padding(0)
						[
							SNew(SCheckBox)
								.Style(FCoreStyle::Get(), "ToggleButtonCheckbox")
								.IsEnabled(this, &SLogViewerProFileMenu::HasAnyTextInTabName)
								.IsChecked(ECheckBoxState::Checked)
								.OnCheckStateChanged(this, &SLogViewerProFileMenu::ExecuteSaveNamedTab)
								[
									SNew(SBox)
										.VAlign(VAlign_Center)
										.HAlign(HAlign_Center)
										.Padding(FMargin(4.0, 2.0))
										[
											SNew(STextBlock)
												.Text(LOCTEXT("EditBoxTabNameSave", "Save named tab"))
										]
								]
						]
						+ SHorizontalBox::Slot()
						.Padding(0)
						[
							SNew(SCheckBox)
								.Style(FCoreStyle::Get(), "ToggleButtonCheckbox")
								.IsChecked(ECheckBoxState::Unchecked)
								.OnCheckStateChanged(this, &SLogViewerProFileMenu::ExecuteCancelNamedTab)
								[
									SNew(SBox)
										.VAlign(VAlign_Center)
										.HAlign(HAlign_Center)
										.Padding(FMargin(4.0, 2.0))
										[
											SNew(STextBlock)
												.Text(LOCTEXT("EditBoxTabNameCancel", "Cancel"))
										]
								]
						]
				]
		];

	if (!MainWidget->GetNamedTabName().IsEmpty())
	{
		TabNameTextBox->SetText(FText::FromString(MainWidget->GetNamedTabName()));
	}

	SaveNamedTabWindow->SetContent(SaveNamedTabContent.ToSharedRef());

	FSlateApplication::Get().AddModalWindow(SaveNamedTabWindow.ToSharedRef(), FGlobalTabmanager::Get()->GetRootWindow());
	//return FReply::Handled();
}

void SLogViewerProFileMenu::ExecuteSaveNamedTab(ECheckBoxState CheckState)
{
	if (!MainWidget) //Not possible
	{
		return;
	}

	MainWidget->SaveAsNamedTab(TabNameTextBox->GetText());

	SaveNamedTabWindow->RequestDestroyWindow();
}



bool SLogViewerProFileMenu::HasAnyTextInTabName() const
{
	return !TabNameTextBox->GetText().IsEmpty();
}

void SLogViewerProFileMenu::ExecuteCancelNamedTab(ECheckBoxState CheckState)
{
	SaveNamedTabWindow->RequestDestroyWindow();
}



FString SLogViewerProFileMenu::GetDefaultNamedTabsFolderPath() const
{
	FString Path = FPaths::ProjectConfigDir() + TEXT("LogViewerPro") + TEXT("/NamedTabs");
	FPaths::NormalizeDirectoryName(Path);
	return Path;
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE