// Copyright Dmitrii Labadin 2023

#pragma once

#include "CoreMinimal.h"
#include "SlateFwd.h"
#include "LogViewerProStructs.h"
#include "Serialization/JsonSerializerMacros.h"

#include "Widgets/Input/SComboButton.h"


class SLogViewerProWidgetMain;




class SLogViewerProFileMenu : public SComboButton
{
public:

	SLATE_BEGIN_ARGS(SLogViewerProFileMenu)
		: _MainWidget()
	{}
		SLATE_ARGUMENT(SLogViewerProWidgetMain*, MainWidget)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);


private:

	SLogViewerProWidgetMain* MainWidget;

	TSharedRef<SWidget> GenerateFileMenu();
	FString GetDefaultNamedTabsFolderPath() const;
	void OpenPopup_SaveNamedTab();
	
	void ExecuteSaveNamedTab(ECheckBoxState CheckState);
	bool HasAnyTextInTabName() const;
	void ExecuteCancelNamedTab(ECheckBoxState CheckState);
	/** Pointer to the parent window, so we know to destroy it when done */
	TWeakPtr<SWindow> SaveNamedTabWindowPtr;
	TSharedPtr<SWindow> SaveNamedTabWindow;

	TSharedPtr< SEditableTextBox > TabNameTextBox;

	//@TODO obviously requires changes in responsibilities, but not for this update, it would be too much
	void OpenLog();
	void SaveLog();

};
