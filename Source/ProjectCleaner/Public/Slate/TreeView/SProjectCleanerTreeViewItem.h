﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FProjectCleanerTreeViewItem;

class SProjectCleanerTreeViewItem final : public SMultiColumnTableRow<TSharedPtr<FProjectCleanerTreeViewItem>>
{
	SLATE_BEGIN_ARGS(SProjectCleanerTreeViewItem)
		{
		}

		SLATE_ARGUMENT(TSharedPtr<FProjectCleanerTreeViewItem>, TreeItem)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable);
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override;
private:
	const FSlateBrush* GetFolderIcon() const;
	FSlateColor GetFolderColor() const;

	TSharedPtr<FProjectCleanerTreeViewItem> TreeItem;
};
