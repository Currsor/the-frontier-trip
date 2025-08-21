// Copyright Epic Games, Inc. All Rights Reserved.

#include "CurrsorToolStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FCurrsorToolStyle::StyleInstance = nullptr;

void FCurrsorToolStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FCurrsorToolStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FCurrsorToolStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("CurrsorToolStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon64x64(64.0f, 64.0f);

TSharedRef< FSlateStyleSet > FCurrsorToolStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("CurrsorToolStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("CurrsorTool")->GetBaseDir() / TEXT("Resources"));

	Style->Set("CurrsorTool.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("Ue"), Icon64x64));

	return Style;
}

void FCurrsorToolStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FCurrsorToolStyle::Get()
{
	return *StyleInstance;
}
