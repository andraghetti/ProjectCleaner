﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FProjectCleanerCmds final : public TCommands<FProjectCleanerCmds>
{
public:
	FProjectCleanerCmds();
	virtual void RegisterCommands() override;

	// general
	TSharedPtr<FUICommandInfo> OpenProjectCleanerWindow;

	// tabs context menu
	TSharedPtr<FUICommandInfo> TabNonEngineTryOpenFile;
	TSharedPtr<FUICommandInfo> TabNonEngineDeleteFile;
	
	TSharedPtr<FUICommandInfo> ShowInContentBrowser;
	TSharedPtr<FUICommandInfo> OpenAsset;
	TSharedPtr<FUICommandInfo> OpenFile;
	TSharedPtr<FUICommandInfo> OpenDocs;
	
	TSharedPtr<FUICommandInfo> DeleteAsset;
	TSharedPtr<FUICommandInfo> IncludeAsset;
	TSharedPtr<FUICommandInfo> IncludePath;
	TSharedPtr<FUICommandInfo> ExcludeAsset;
	TSharedPtr<FUICommandInfo> ExcludeByType;
	TSharedPtr<FUICommandInfo> ExcludePath;
};
