﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PjcTypes.h"
#include "Widgets/SCompoundWidget.h"

class UPjcSubsystem;

class SPjcTabAssetsUnused final : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPjcTabAssetsUnused) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual ~SPjcTabAssetsUnused() override;
private:
	TSharedRef<SWidget> CreateToolbar() const;
	TSharedRef<SWidget> GetTreeOptionsBtnContent();
	TSharedPtr<SWidget> GetTreeContextMenu() const;
	TSharedPtr<SWidget> GetContentBrowserContextMenu(const TArray<FAssetData>& Assets) const;
	TSharedRef<SHeaderRow> GetTreeHeaderRow() const;
	TSharedRef<ITableRow> OnTreeGenerateRow(TSharedPtr<FPjcTreeItem> Item, const TSharedRef<STableViewBase>& OwnerTable) const;
	TSharedPtr<FPjcTreeItem> CreateTreeItem(const FString& InFolderPath) const;
	FSlateColor GetTreeOptionsBtnForegroundColor() const;
	FText GetTreeSummaryText() const;
	void TreeItemsUpdate();
	void TreeItemsFilter();
	void TreeItemsCollapseAll();
	void TreeItemsExpandAll();
	void TreeItemExpandParentsRecursive(const TSharedPtr<FPjcTreeItem>& Item) const;
	void TreeItemMakeVisibleParentsRecursive(const TSharedPtr<FPjcTreeItem>& Item) const;
	void SetTreeItemVisibility(const TSharedPtr<FPjcTreeItem>& Item) const;
	void SetTreeItemExpansion(const TSharedPtr<FPjcTreeItem>& Item);
	void OnTreeGetChildren(TSharedPtr<FPjcTreeItem> Item, TArray<TSharedPtr<FPjcTreeItem>>& OutChildren);
	void OnTreeExpansionChanged(TSharedPtr<FPjcTreeItem> Item, const bool bIsExpanded) const;
	void OnTreeSearchTextChanged(const FText& InText);
	void OnTreeSearchTextCommitted(const FText& InText, ETextCommit::Type Type);
	bool TreeItemContainsSearchText(const TSharedPtr<FPjcTreeItem>& Item) const;

	FText SearchText;
	TSharedPtr<FUICommandList> Cmds;
	UPjcSubsystem* SubsystemPtr = nullptr;
	TSharedPtr<FPjcTreeItem> TreeRootItem;
	TSharedPtr<SComboButton> TreeOptionBtn;
	TArray<TSharedPtr<FPjcTreeItem>> TreeItems;
	TSet<TSharedPtr<FPjcTreeItem>> TreeItemsExpanded;
	TSharedPtr<STreeView<TSharedPtr<FPjcTreeItem>>> TreeView;
};
