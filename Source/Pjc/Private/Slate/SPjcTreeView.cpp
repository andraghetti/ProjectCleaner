﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#include "Slate/SPjcTreeView.h"
#include "Slate/SPjcItemTree.h"
#include "PjcSubsystem.h"
#include "PjcConstants.h"
#include "PjcStyles.h"
#include "PjcCmds.h"
// Engine Headers
// #include "ObjectTools.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"

void SPjcTreeView::Construct(const FArguments& InArgs)
{
	SubsystemPtr = GEditor->GetEditorSubsystem<UPjcSubsystem>();

	Cmds = MakeShareable(new FUICommandList);

	Cmds->MapAction(
		FPjcCmds::Get().DeleteEmptyFolders,
		FExecuteAction::CreateLambda([&]()
		{
			const FText Title = FText::FromString(TEXT("Delete Empty Folder"));
			const FText Context = FText::FromString(TEXT("Are you sure you want to delete all empty folders?"));

			const EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::YesNo, Context, &Title);
			if (ReturnType == EAppReturnType::Cancel || ReturnType == EAppReturnType::No) return;

			// UPjcSubsystem::DeleteFoldersEmpty(true, true);

			// UpdateData();
		})
	);

	Cmds->MapAction(
		FPjcCmds::Get().PathsExclude,
		FExecuteAction::CreateLambda([&]()
		{
			// todo:ashe23
			UE_LOG(LogTemp, Warning, TEXT("Excluding selected paths"))
		}),
		FCanExecuteAction::CreateLambda([&]()
		{
			return TreeView.IsValid() && TreeView->GetSelectedItems().Num();
		})
	);

	Cmds->MapAction(
		FPjcCmds::Get().PathsInclude,
		FExecuteAction::CreateLambda([&]()
		{
			// todo:ashe23
			UE_LOG(LogTemp, Warning, TEXT("Including selected paths"))
		}),
		FCanExecuteAction::CreateLambda([&]()
		{
			return TreeView.IsValid() && TreeView->GetSelectedItems().Num();
		})
	);

	Cmds->MapAction(
		FPjcCmds::Get().ClearSelection,
		FExecuteAction::CreateLambda([&]()
		{
			if (TreeView.IsValid())
			{
				TreeView->ClearSelection();
				TreeView->ClearHighlightedItems();
			}
		}),
		FCanExecuteAction::CreateLambda([&]()
		{
			return TreeView.IsValid() && TreeView->GetSelectedItems().Num() > 0;
		})
	);

	SAssignNew(TreeView, STreeView<TSharedPtr<FPjcTreeItem>>)
	.TreeItemsSource(&TreeItems)
	.SelectionMode(ESelectionMode::Multi)
	.OnGenerateRow(this, &SPjcTreeView::OnTreeGenerateRow)
	.OnGetChildren(this, &SPjcTreeView::OnTreeGetChildren)
	.OnContextMenuOpening_Raw(this, &SPjcTreeView::GetTreeContextMenu)
	.OnSelectionChanged_Raw(this, &SPjcTreeView::OnTreeSelectionChanged)
	.OnExpansionChanged_Raw(this, &SPjcTreeView::OnTreeExpansionChanged)
	.HeaderRow(GetTreeHeaderRow());

	UpdateData();
	UpdateView();

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight().Padding(3.0f)
		[
			CreateToolbar()
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(3.0f)
		[
			SNew(SSeparator).Thickness(5.0f)
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(5.0f)
		[
			SNew(SSearchBox)
			.HintText(FText::FromString(TEXT("Search Folders...")))
			.OnTextChanged_Raw(this, &SPjcTreeView::OnTreeSearchTextChanged)
			.OnTextCommitted_Raw(this, &SPjcTreeView::OnTreeSearchTextCommitted)
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(5.0f, 0.0f, 0.0f, 2.1f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SImage)
				.Image(FEditorStyle::GetBrush("ContentBrowser.AssetTreeFolderOpen"))
				.ColorAndOpacity(FPjcStyles::Get().GetSlateColor("ProjectCleaner.Color.Red"))
			]
			+ SHorizontalBox::Slot().AutoWidth().Padding(FMargin{0.0f, 2.0f, 5.0f, 0.0f})
			[
				SNew(STextBlock).Text(FText::FromString(TEXT(" - Empty Folders")))
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SImage)
				.Image(FEditorStyle::GetBrush("ContentBrowser.AssetTreeFolderOpen"))
				.ColorAndOpacity(FPjcStyles::Get().GetSlateColor("ProjectCleaner.Color.Yellow"))
			]
			+ SHorizontalBox::Slot().AutoWidth().Padding(FMargin{0.0f, 2.0f, 5.0f, 0.0f})
			[
				SNew(STextBlock).Text(FText::FromString(TEXT(" - Excluded Folders")))
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SImage)
				.Image(FEditorStyle::GetBrush("ContentBrowser.AssetTreeFolderOpen"))
				.ColorAndOpacity(FPjcStyles::Get().GetSlateColor("ProjectCleaner.Color.Blue"))
			]
			+ SHorizontalBox::Slot().AutoWidth().Padding(FMargin{0.0f, 2.0f, 5.0f, 0.0f})
			[
				SNew(STextBlock).Text(FText::FromString(TEXT(" - Engine Folders")))
			]
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(5.0f)
		[
			SNew(SSeparator).Thickness(3.0f)
		]
		+ SVerticalBox::Slot().FillHeight(1.0f).Padding(5.0f)
		[
			SNew(SScrollBox)
			.ScrollWhenFocusChanges(EScrollWhenFocusChanges::NoScroll)
			.AnimateWheelScrolling(true)
			.AllowOverscroll(EAllowOverscroll::No)
			+ SScrollBox::Slot()
			[
				TreeView.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1.0f).HAlign(HAlign_Left).VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text_Raw(this, &SPjcTreeView::GetTreeSummaryText)
			]
			+ SHorizontalBox::Slot().FillWidth(1.0f).HAlign(HAlign_Center).VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text_Raw(this, &SPjcTreeView::GetTreeSelectionText)
			]
			+ SHorizontalBox::Slot().FillWidth(1.0f).HAlign(HAlign_Right).VAlign(VAlign_Center)
			[
				SNew(SComboButton)
				.ContentPadding(0)
				.ForegroundColor_Raw(this, &SPjcTreeView::GetTreeOptionsBtnForegroundColor)
				.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
				.OnGetMenuContent(this, &SPjcTreeView::GetTreeBtnOptionsContent)
				.ButtonContent()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
					[
						SNew(SImage).Image(FEditorStyle::GetBrush("GenericViewButton"))
					]
					+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f, 0.0f, 0.0f, 0.0f).VAlign(VAlign_Center)
					[
						SNew(STextBlock).Text(FText::FromString(TEXT("View Options")))
					]
				]
			]
		]
	];
}

void SPjcTreeView::UpdateData()
{
	TArray<FAssetData> AssetsAll;
	TArray<FAssetData> AssetsUsed;
	TArray<FAssetData> AssetsUnused;
	TArray<FString> FoldersTotal;

	UPjcSubsystem::GetAssetsAll(AssetsAll);
	UPjcSubsystem::GetAssetsUsed(AssetsUsed, false);
	UPjcSubsystem::GetAssetsUnused(AssetsUnused, false);
	UPjcSubsystem::GetFolders(FPaths::ProjectContentDir(), true, FoldersTotal);
	NumFoldersTotal = FoldersTotal.Num();

	MapNumAssetsAllByPath.Reset();
	MapNumAssetsUsedByPath.Reset();
	MapNumAssetsUnusedByPath.Reset();
	MapSizeAssetsAllByPath.Reset();
	MapSizeAssetsUsedByPath.Reset();
	MapSizeAssetsUnusedByPath.Reset();

	for (const FAssetData& Asset : AssetsAll)
	{
		const FString AssetPath = Asset.PackagePath.ToString();
		const int64 AssetSize = UPjcSubsystem::GetAssetSize(Asset);
		UpdateMapInfo(MapNumAssetsAllByPath, MapSizeAssetsAllByPath, AssetPath, AssetSize);
	}

	for (const FAssetData& Asset : AssetsUsed)
	{
		const FString AssetPath = Asset.PackagePath.ToString();
		const int64 AssetSize = UPjcSubsystem::GetAssetSize(Asset);
		UpdateMapInfo(MapNumAssetsUsedByPath, MapSizeAssetsUsedByPath, AssetPath, AssetSize);
	}

	for (const FAssetData& Asset : AssetsUnused)
	{
		const FString AssetPath = Asset.PackagePath.ToString();
		const int64 AssetSize = UPjcSubsystem::GetAssetSize(Asset);
		UpdateMapInfo(MapNumAssetsUnusedByPath, MapSizeAssetsUnusedByPath, AssetPath, AssetSize);
	}

	// TSet<TSharedPtr<FPjcTreeItem>> CachedExpandedItems;
	// TreeView->GetExpandedItems(CachedExpandedItems);

	RootItem.Reset();
	// TreeItems.Reset();

	// creating root item
	const FString PathContentDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());

	RootItem = MakeShareable(new FPjcTreeItem);
	if (!RootItem.IsValid()) return;

	RootItem->FolderPath = PjcConstants::PathRoot.ToString();
	RootItem->FolderName = TEXT("Content");
	RootItem->bIsDev = false;
	RootItem->bIsRoot = true;
	RootItem->bIsEmpty = UPjcSubsystem::FolderIsEmpty(PathContentDir);
	RootItem->bIsExcluded = UPjcSubsystem::FolderIsExcluded(PathContentDir);
	RootItem->bIsExpanded = true;
	RootItem->bIsVisible = true;
	RootItem->NumAssetsTotal = AssetsAll.Num();
	RootItem->NumAssetsUsed = AssetsUsed.Num();
	RootItem->NumAssetsUnused = AssetsUnused.Num();
	RootItem->SizeAssetsUnused = UPjcSubsystem::GetAssetsTotalSize(AssetsUnused);
	RootItem->PercentageUnused = RootItem->NumAssetsTotal == 0 ? 0 : RootItem->NumAssetsUnused * 100.0f / RootItem->NumAssetsTotal;
	RootItem->PercentageUnusedNormalized = FMath::GetMappedRangeValueClamped(FVector2D{0.0f, 100.0f}, FVector2D{0.0f, 1.0f}, RootItem->PercentageUnused);
	RootItem->Parent = nullptr;

	// TreeItems.Emplace(RootItem);
	// TreeView->SetItemExpansion(RootItem, RootItem->bIsExpanded);

	// filling whole tree
	TArray<TSharedPtr<FPjcTreeItem>> Stack;
	Stack.Push(RootItem);

	while (Stack.Num() > 0)
	{
		const auto CurrentItem = Stack.Pop(false);

		TArray<FString> SubPaths;
		UPjcSubsystem::GetModuleAssetRegistry().Get().GetSubPaths(CurrentItem->FolderPath, SubPaths, false);

		for (const auto& SubPath : SubPaths)
		{
			const TSharedPtr<FPjcTreeItem> SubItem = MakeShareable(new FPjcTreeItem);
			if (!SubItem.IsValid()) continue;

			SubItem->FolderPath = SubPath;
			SubItem->FolderName = FPaths::GetPathLeaf(SubItem->FolderPath);
			SubItem->bIsDev = SubItem->FolderPath.StartsWith(PjcConstants::PathDevelopers.ToString());
			SubItem->bIsRoot = false;
			SubItem->bIsEmpty = UPjcSubsystem::FolderIsEmpty(SubItem->FolderPath);
			SubItem->bIsExcluded = UPjcSubsystem::FolderIsExcluded(SubItem->FolderPath);
			SubItem->bIsExpanded = false;
			SubItem->bIsVisible = true;
			SubItem->NumAssetsTotal = MapNumAssetsAllByPath.Contains(SubItem->FolderPath) ? MapNumAssetsAllByPath[SubItem->FolderPath] : 0;
			SubItem->NumAssetsUsed = MapNumAssetsUsedByPath.Contains(SubItem->FolderPath) ? MapNumAssetsUsedByPath[SubItem->FolderPath] : 0;
			SubItem->NumAssetsUnused = MapNumAssetsUnusedByPath.Contains(SubItem->FolderPath) ? MapNumAssetsUnusedByPath[SubItem->FolderPath] : 0;
			SubItem->SizeAssetsUnused = MapSizeAssetsUnusedByPath.Contains(SubItem->FolderPath) ? MapSizeAssetsUnusedByPath[SubItem->FolderPath] : 0.0f;
			SubItem->PercentageUnused = SubItem->NumAssetsTotal == 0 ? 0 : SubItem->NumAssetsUnused * 100.0f / SubItem->NumAssetsTotal;
			SubItem->PercentageUnusedNormalized = FMath::GetMappedRangeValueClamped(FVector2D{0.0f, 100.0f}, FVector2D{0.0f, 1.0f}, SubItem->PercentageUnused);
			SubItem->Parent = CurrentItem;

			CurrentItem->SubItems.Emplace(SubItem);
			Stack.Emplace(SubItem);
		}
	}
}

void SPjcTreeView::UpdateView()
{
	if (!RootItem || !SubsystemPtr || !TreeView.IsValid()) return;

	TArray<TSharedPtr<FPjcTreeItem>> Stack;
	Stack.Push(RootItem);

	while (Stack.Num() > 0)
	{
		const auto& CurrentItem = Stack.Pop(false);

		SetItemVisibility(CurrentItem);
		SetItemExpansion(CurrentItem);

		for (const auto& SubItem : CurrentItem->SubItems)
		{
			Stack.Push(SubItem);
		}
	}

	TreeItems.Reset();
	TreeItems.Add(RootItem);

	SortTreeItems(false);

	TreeView->RebuildList();
}

SPjcTreeView::FPjcDelegateSelectionChanged& SPjcTreeView::OnSelectionChanged()
{
	return DelegateSelectionChanged;
}

const TSet<FString>& SPjcTreeView::GetSelectedPaths() const
{
	return SelectedPaths;
}

TSharedRef<SWidget> SPjcTreeView::GetTreeBtnOptionsContent()
{
	const TSharedPtr<FExtender> Extender;
	FMenuBuilder MenuBuilder(true, Cmds, Extender, true);

	MenuBuilder.AddMenuSeparator(TEXT("View"));

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Show Folders Empty")),
		FText::FromString(TEXT("Show empty folders in tree view")),
		FSlateIcon(),
		FUIAction
		(
			FExecuteAction::CreateLambda([&]
			{
				SubsystemPtr->bShowFoldersEmpty = !SubsystemPtr->bShowFoldersEmpty;
				SubsystemPtr->PostEditChange();

				UpdateView();
			}),
			FCanExecuteAction::CreateLambda([&]()
			{
				return SubsystemPtr != nullptr;
			}),
			FGetActionCheckState::CreateLambda([&]()
			{
				return SubsystemPtr->bShowFoldersEmpty ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			})
		),
		NAME_None,
		EUserInterfaceActionType::ToggleButton
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Show Folders Excluded")),
		FText::FromString(TEXT("Show excluded folders in tree view")),
		FSlateIcon(),
		FUIAction
		(
			FExecuteAction::CreateLambda([&]
			{
				SubsystemPtr->bShowFoldersExcluded = !SubsystemPtr->bShowFoldersExcluded;
				SubsystemPtr->PostEditChange();

				UpdateView();
			}),
			FCanExecuteAction::CreateLambda([&]()
			{
				return SubsystemPtr != nullptr;
			}),
			FGetActionCheckState::CreateLambda([&]()
			{
				return SubsystemPtr->bShowFoldersExcluded ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			})
		),
		NAME_None,
		EUserInterfaceActionType::ToggleButton
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Show Folders Used")),
		FText::FromString(TEXT("Show folders that dont contain unused assets in tree view")),
		FSlateIcon(),
		FUIAction
		(
			FExecuteAction::CreateLambda([&]
			{
				SubsystemPtr->bShowFoldersUsed = !SubsystemPtr->bShowFoldersUsed;
				SubsystemPtr->PostEditChange();

				UpdateView();
			}),
			FCanExecuteAction::CreateLambda([&]()
			{
				return SubsystemPtr != nullptr;
			}),
			FGetActionCheckState::CreateLambda([&]()
			{
				return SubsystemPtr->bShowFoldersUsed ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			})
		),
		NAME_None,
		EUserInterfaceActionType::ToggleButton
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Show Folders Engine Generated")),
		FText::FromString(TEXT("Show engine generated folders in tree view")),
		FSlateIcon(),
		FUIAction
		(
			FExecuteAction::CreateLambda([&]
			{
				SubsystemPtr->bShowFoldersEngine = !SubsystemPtr->bShowFoldersEngine;
				SubsystemPtr->PostEditChange();

				UpdateView();
			}),
			FCanExecuteAction::CreateLambda([&]()
			{
				return SubsystemPtr != nullptr;
			}),
			FGetActionCheckState::CreateLambda([&]()
			{
				return SubsystemPtr->bShowFoldersEngine ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			})
		),
		NAME_None,
		EUserInterfaceActionType::ToggleButton
	);

	MenuBuilder.AddSeparator();

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Expand Selected Recursive")),
		FText::FromString(TEXT("Expand selected folders recursively")),
		FSlateIcon(),
		FUIAction
		(
			FExecuteAction::CreateLambda([&]
			{
				// const auto SelectedItems = TreeView->GetSelectedItems();
				//
				// for (const auto& SelectedItem : SelectedItems)
				// {
				// 	if (!SelectedItem.IsValid()) continue;
				// 	ChangeItemExpansionRecursive(SelectedItem, true);
				// }
				//
				// TreeView->RebuildList();
			}),
			FCanExecuteAction::CreateLambda([&]()
			{
				return TreeView.IsValid() && TreeView->GetSelectedItems().Num() > 0;
			})
		),
		NAME_None,
		EUserInterfaceActionType::Button
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Collapse Selected Recursive")),
		FText::FromString(TEXT("Collapse selected folders recursively")),
		FSlateIcon(),
		FUIAction
		(
			FExecuteAction::CreateLambda([&]
			{
				// const auto SelectedItems = TreeView->GetSelectedItems();
				//
				// for (const auto& SelectedItem : SelectedItems)
				// {
				// 	// if (!SelectedItem.IsValid()) continue;
				// 	// ChangeItemExpansionRecursive(SelectedItem, false);
				// }
				//
				// TreeView->RebuildList();
			}),
			FCanExecuteAction::CreateLambda([&]()
			{
				return TreeView.IsValid() && TreeView->GetSelectedItems().Num() > 0;
			})
		),
		NAME_None,
		EUserInterfaceActionType::Button
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Expand All")),
		FText::FromString(TEXT("Expand all folders recursively")),
		FSlateIcon(),
		FUIAction
		(
			FExecuteAction::CreateLambda([&]
			{
				// ChangeItemExpansionRecursive(RootItem, true);
				// TreeView->RebuildList();
			}),
			FCanExecuteAction::CreateLambda([&]()
			{
				return RootItem.IsValid() && TreeView.IsValid();
			})
		),
		NAME_None,
		EUserInterfaceActionType::Button
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Collapse All")),
		FText::FromString(TEXT("Collapse all folders recursively")),
		FSlateIcon(),
		FUIAction
		(
			FExecuteAction::CreateLambda([&]
			{
				// ChangeItemExpansionRecursive(RootItem, false);
				// TreeView->RebuildList();
			}),
			FCanExecuteAction::CreateLambda([&]()
			{
				return RootItem.IsValid() && TreeView.IsValid();
			})
		),
		NAME_None,
		EUserInterfaceActionType::Button
	);

	return MenuBuilder.MakeWidget();
}

TSharedPtr<SWidget> SPjcTreeView::GetTreeContextMenu() const
{
	FMenuBuilder MenuBuilder{true, Cmds};

	MenuBuilder.BeginSection(TEXT("PjcSectionPathActions"), FText::FromString(TEXT("Actions")));
	{
		MenuBuilder.AddMenuEntry(FPjcCmds::Get().PathsExclude);
		MenuBuilder.AddMenuEntry(FPjcCmds::Get().PathsInclude);
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

TSharedRef<SHeaderRow> SPjcTreeView::GetTreeHeaderRow()
{
	const FMargin HeaderPadding{5.0f};

	return
		SNew(SHeaderRow)
		+ SHeaderRow::Column(TEXT("Path"))
		  .HAlignHeader(HAlign_Center)
		  .VAlignHeader(VAlign_Center)
		  .HeaderContentPadding(HeaderPadding)
		  .FillWidth(0.4f)
		  .OnSort_Raw(this, &SPjcTreeView::OnTreeSort)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Path")))
			.ColorAndOpacity(FPjcStyles::Get().GetSlateColor("ProjectCleaner.Color.Green"))
			.Font(FPjcStyles::GetFont("Light", 10.0f))
		]
		+ SHeaderRow::Column(TEXT("NumAssetsTotal"))
		  .HAlignHeader(HAlign_Center)
		  .VAlignHeader(VAlign_Center)
		  .HeaderContentPadding(HeaderPadding)
		  .FillWidth(0.1f)
		  .OnSort_Raw(this, &SPjcTreeView::OnTreeSort)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Total")))
			.ToolTipText(FText::FromString(TEXT("Total number of assets in current path")))
			.ColorAndOpacity(FPjcStyles::Get().GetSlateColor("ProjectCleaner.Color.Green"))
			.Font(FPjcStyles::GetFont("Light", 10.0f))
		]
		+ SHeaderRow::Column(TEXT("NumAssetsUsed"))
		  .HAlignHeader(HAlign_Center)
		  .VAlignHeader(VAlign_Center)
		  .HeaderContentPadding(HeaderPadding)
		  .FillWidth(0.1f)
		  .OnSort_Raw(this, &SPjcTreeView::OnTreeSort)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Used")))
			.ToolTipText(FText::FromString(TEXT("Total number of used assets in current path")))
			.ColorAndOpacity(FPjcStyles::Get().GetSlateColor("ProjectCleaner.Color.Green"))
			.Font(FPjcStyles::GetFont("Light", 10.0f))
		]
		+ SHeaderRow::Column(TEXT("NumAssetsUnused"))
		  .HAlignHeader(HAlign_Center)
		  .VAlignHeader(VAlign_Center)
		  .HeaderContentPadding(HeaderPadding)
		  .FillWidth(0.1f)
		  .OnSort_Raw(this, &SPjcTreeView::OnTreeSort)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Unused")))
			.ToolTipText(FText::FromString(TEXT("Total number of unused assets in current path")))
			.ColorAndOpacity(FPjcStyles::Get().GetSlateColor("ProjectCleaner.Color.Green"))
			.Font(FPjcStyles::GetFont("Light", 10.0f))
		]
		+ SHeaderRow::Column(TEXT("UnusedPercent"))
		  .HAlignHeader(HAlign_Center)
		  .VAlignHeader(VAlign_Center)
		  .HeaderContentPadding(HeaderPadding)
		  .FillWidth(0.15f)
		  .OnSort_Raw(this, &SPjcTreeView::OnTreeSort)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Unused %")))
			.ToolTipText(FText::FromString(TEXT("Percentage of unused assets number relative to total assets number in current path")))
			.ColorAndOpacity(FPjcStyles::Get().GetSlateColor("ProjectCleaner.Color.Green"))
			.Font(FPjcStyles::GetFont("Light", 10.0f))
		]
		+ SHeaderRow::Column(TEXT("UnusedSize"))
		  .HAlignHeader(HAlign_Center)
		  .VAlignHeader(VAlign_Center)
		  .HeaderContentPadding(HeaderPadding)
		  .FillWidth(0.15f)
		  .OnSort_Raw(this, &SPjcTreeView::OnTreeSort)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Unused Size")))
			.ToolTipText(FText::FromString(TEXT("Total size of unused assets in current path")))
			.ColorAndOpacity(FPjcStyles::Get().GetSlateColor("ProjectCleaner.Color.Green"))
			.Font(FPjcStyles::GetFont("Light", 10.0f))
		];
}

TSharedRef<ITableRow> SPjcTreeView::OnTreeGenerateRow(TSharedPtr<FPjcTreeItem> Item, const TSharedRef<STableViewBase>& OwnerTable) const
{
	return SNew(SPjcItemTree, OwnerTable).Item(Item).HightlightText(SearchText);
}

void SPjcTreeView::OnTreeGetChildren(TSharedPtr<FPjcTreeItem> Item, TArray<TSharedPtr<FPjcTreeItem>>& OutChildren)
{
	if (!Item.IsValid()) return;

	for (const auto& SubItem : Item->SubItems)
	{
		if (SubItem->bIsVisible)
		{
			OutChildren.Add(SubItem);
		}
	}
}

void SPjcTreeView::OnTreeSearchTextChanged(const FText& InText)
{
	SearchText = InText;
	// UpdateView();
}

void SPjcTreeView::OnTreeSearchTextCommitted(const FText& InText, ETextCommit::Type Type)
{
	SearchText = InText;
	// UpdateView();
}

void SPjcTreeView::OnTreeSort(EColumnSortPriority::Type SortPriority, const FName& ColumnName, EColumnSortMode::Type InSortMode)
{
	if (!RootItem.IsValid() || !TreeView.IsValid()) return;

	LastSortedColumn = ColumnName;

	SortTreeItems(true);

	TreeView->RebuildList();
}

void SPjcTreeView::OnTreeSelectionChanged(TSharedPtr<FPjcTreeItem> Selection, ESelectInfo::Type SelectInfo)
{
	if (!TreeView.IsValid()) return;

	const auto ItemsSelected = TreeView->GetSelectedItems();

	SelectedPaths.Empty(ItemsSelected.Num());

	for (const auto& Item : ItemsSelected)
	{
		if (!Item.IsValid()) continue;

		SelectedPaths.Emplace(Item->FolderPath);
	}

	if (DelegateSelectionChanged.IsBound())
	{
		DelegateSelectionChanged.Execute();
	}
}

void SPjcTreeView::OnTreeExpansionChanged(TSharedPtr<FPjcTreeItem> Item, bool bIsExpanded)
{
	if (!TreeView.IsValid()) return;

	Item->bIsExpanded = bIsExpanded;
	TreeView->SetItemExpansion(Item, bIsExpanded);
	TreeView->RebuildList();
}

void SPjcTreeView::SetItemVisibility(const TSharedPtr<FPjcTreeItem>& Item) const
{
	if (!Item.IsValid() || !SubsystemPtr) return;

	if (Item->bIsRoot)
	{
		Item->bIsVisible = true;
		return;
	}

	if (Item->bIsDev && !SubsystemPtr->bShowFoldersEngine)
	{
		Item->bIsVisible = false;
		return;
	}

	if (Item->bIsExcluded && !SubsystemPtr->bShowFoldersExcluded)
	{
		Item->bIsVisible = false;
		return;
	}

	if (Item->bIsEmpty && !Item->bIsDev && !Item->bIsExcluded && !SubsystemPtr->bShowFoldersEmpty)
	{
		Item->bIsVisible = false;
		return;
	}

	if (Item->NumAssetsUsed > 0 && Item->NumAssetsUnused == 0 && !Item->bIsExcluded && !SubsystemPtr->bShowFoldersUsed)
	{
		Item->bIsVisible = false;
		return;
	}

	Item->bIsVisible = true;
}

void SPjcTreeView::SetItemExpansion(const TSharedPtr<FPjcTreeItem>& Item) const
{
	if (!Item.IsValid() || !TreeView.IsValid()) return;

	if (Item->bIsRoot)
	{
		Item->bIsExpanded = true;
		TreeView->SetItemExpansion(Item, Item->bIsExpanded);
		return;
	}

	TSet<TSharedPtr<FPjcTreeItem>> CachedExpandedItems;
	TreeView->GetExpandedItems(CachedExpandedItems);

	for (const auto& ExpandedItem : CachedExpandedItems)
	{
		if (!ExpandedItem.IsValid()) continue;

		if (ExpandedItem->FolderPath.Equals(Item->FolderPath))
		{
			Item->bIsExpanded = true;
			TreeView->SetItemExpansion(Item, Item->bIsExpanded);
		}
	}
}

void SPjcTreeView::SortTreeItems(const bool UpdateColumnSorting)
{
	auto SortTreeItems = [&](auto& SortMode, auto SortFunc)
	{
		if (UpdateColumnSorting)
		{
			SortMode = SortMode == EColumnSortMode::Ascending ? EColumnSortMode::Descending : EColumnSortMode::Ascending;
		}

		TArray<TSharedPtr<FPjcTreeItem>> Stack;
		Stack.Push(RootItem);

		while (Stack.Num() > 0)
		{
			const auto& CurrentItem = Stack.Pop(false);
			if (!CurrentItem.IsValid()) continue;

			TArray<TSharedPtr<FPjcTreeItem>>& SubItems = CurrentItem->SubItems;
			SubItems.Sort(SortFunc);

			Stack.Append(CurrentItem->SubItems);
		}
	};

	if (LastSortedColumn.IsEqual(TEXT("Path")))
	{
		SortTreeItems(ColumnPathSortMode, [&](const TSharedPtr<FPjcTreeItem>& Item1, const TSharedPtr<FPjcTreeItem>& Item2)
		{
			return ColumnPathSortMode == EColumnSortMode::Ascending ? Item1->FolderPath < Item2->FolderPath : Item1->FolderPath > Item2->FolderPath;
		});
	}

	if (LastSortedColumn.IsEqual(TEXT("NumAssetsTotal")))
	{
		SortTreeItems(ColumnAssetsTotalSortMode, [&](const TSharedPtr<FPjcTreeItem>& Item1, const TSharedPtr<FPjcTreeItem>& Item2)
		{
			return ColumnAssetsTotalSortMode == EColumnSortMode::Ascending ? Item1->NumAssetsTotal < Item2->NumAssetsTotal : Item1->NumAssetsTotal > Item2->NumAssetsTotal;
		});
	}

	if (LastSortedColumn.IsEqual(TEXT("NumAssetsUsed")))
	{
		SortTreeItems(ColumnAssetsUsedSortMode, [&](const TSharedPtr<FPjcTreeItem>& Item1, const TSharedPtr<FPjcTreeItem>& Item2)
		{
			return ColumnAssetsUsedSortMode == EColumnSortMode::Ascending ? Item1->NumAssetsUsed < Item2->NumAssetsUsed : Item1->NumAssetsUsed > Item2->NumAssetsUsed;
		});
	}

	if (LastSortedColumn.IsEqual(TEXT("NumAssetsUnused")))
	{
		SortTreeItems(ColumnAssetsUnusedSortMode, [&](const TSharedPtr<FPjcTreeItem>& Item1, const TSharedPtr<FPjcTreeItem>& Item2)
		{
			return ColumnAssetsUnusedSortMode == EColumnSortMode::Ascending ? Item1->NumAssetsUnused < Item2->NumAssetsUnused : Item1->NumAssetsUnused > Item2->NumAssetsUnused;
		});
	}

	if (LastSortedColumn.IsEqual(TEXT("UnusedPercent")))
	{
		SortTreeItems(ColumnUnusedPercentSortMode, [&](const TSharedPtr<FPjcTreeItem>& Item1, const TSharedPtr<FPjcTreeItem>& Item2)
		{
			return ColumnUnusedPercentSortMode == EColumnSortMode::Ascending ? Item1->PercentageUnused < Item2->PercentageUnused : Item1->PercentageUnused > Item2->PercentageUnused;
		});
	}

	if (LastSortedColumn.IsEqual(TEXT("UnusedSize")))
	{
		SortTreeItems(ColumnUnusedSizeSortMode, [&](const TSharedPtr<FPjcTreeItem>& Item1, const TSharedPtr<FPjcTreeItem>& Item2)
		{
			return ColumnUnusedSizeSortMode == EColumnSortMode::Ascending ? Item1->SizeAssetsUnused < Item2->SizeAssetsUnused : Item1->SizeAssetsUnused > Item2->SizeAssetsUnused;
		});
	}
}


// void SPjcTreeView::TreeItemsFilter() { }

// void SPjcTreeView::ChangeItemExpansionRecursive(const TSharedPtr<FPjcTreeItem>& Item, const bool bExpansion) const
// {
// 	if (!Item.IsValid()) return;
// 	if (!TreeView.IsValid()) return;
//
// 	TArray<TSharedPtr<FPjcTreeItem>> Stack;
// 	Stack.Push(Item);
//
// 	while (Stack.Num() > 0)
// 	{
// 		const auto CurrentItem = Stack.Pop(false);
//
// 		CurrentItem->bIsExpanded = bExpansion;
// 		TreeView->SetItemExpansion(CurrentItem, bExpansion);
//
// 		for (const auto& SubItem : CurrentItem->SubItems)
// 		{
// 			Stack.Push(SubItem);
// 		}
// 	}
// }

// bool SPjcTreeView::ItemIsExpanded(const TSharedPtr<FPjcTreeItem>& Item, const TSet<TSharedPtr<FPjcTreeItem>>& ExpandedItems)
// {
// 	if (!Item.IsValid()) return false;
// 	if (ExpandedItems.Num() == 0) return false;
//
// 	for (const auto& ExpandedItem : ExpandedItems)
// 	{
// 		if (!ExpandedItem.IsValid()) continue;
//
// 		if (ExpandedItem->FolderPath.Equals(Item->FolderPath))
// 		{
// 			return true;
// 		}
// 	}
//
// 	return false;
// }
//
// bool SPjcTreeView::ItemIsVisible(const TSharedPtr<FPjcTreeItem>& Item) const
// {
// 	if (!SubsystemPtr) return false;
// 	if (Item->bIsRoot) return true;
// 	if (Item->bIsDev && !SubsystemPtr->bShowFoldersEngine) return false;
// 	if (Item->NumAssetsUsed > 0 && Item->NumAssetsUnused == 0 && !SubsystemPtr->bShowFoldersUsed) return false;
// 	if (Item->bIsExcluded && !SubsystemPtr->bShowFoldersExcluded) return false;
// 	if (Item->bIsEmpty && !SubsystemPtr->bShowFoldersEmpty) return false;
//
// 	return true;
// }

// bool SPjcTreeView::ItemContainsSearchText(const TSharedPtr<FPjcTreeItem>& Item) const
// {
// 	if (!Item.IsValid()) return false;
//
// 	TArray<TSharedPtr<FPjcTreeItem>> Stack;
// 	Stack.Push(Item);
//
// 	while (Stack.Num() > 0)
// 	{
// 		const auto CurrentItem = Stack.Pop(false);
//
// 		if (CurrentItem->FolderName.Contains(SearchText.ToString()))
// 		{
// 			return true;
// 		}
//
// 		for (const auto& SubItem : CurrentItem->SubItems)
// 		{
// 			Stack.Push(SubItem);
// 		}
// 	}
//
// 	return false;
// }

TSharedRef<SWidget> SPjcTreeView::CreateToolbar() const
{
	FToolBarBuilder ToolBarBuilder{Cmds, FMultiBoxCustomization::None};
	ToolBarBuilder.BeginSection("PjcSectionActionsPaths");
	{
		ToolBarBuilder.AddToolBarButton(FPjcCmds::Get().DeleteEmptyFolders);
		ToolBarBuilder.AddSeparator();
		ToolBarBuilder.AddToolBarButton(FPjcCmds::Get().PathsExclude);
		ToolBarBuilder.AddToolBarButton(FPjcCmds::Get().PathsInclude);
		ToolBarBuilder.AddSeparator();
		ToolBarBuilder.AddToolBarButton(FPjcCmds::Get().ClearSelection);
	}
	ToolBarBuilder.EndSection();

	return ToolBarBuilder.MakeWidget();
}

FText SPjcTreeView::GetTreeSummaryText() const
{
	return FText::FromString(FString::Printf(TEXT("Folders Total - %d"), NumFoldersTotal));
}

FText SPjcTreeView::GetTreeSelectionText() const
{
	const int32 NumItemsSelected = TreeView.IsValid() ? TreeView->GetSelectedItems().Num() : 0;

	if (NumItemsSelected > 0)
	{
		return FText::FromString(FString::Printf(TEXT("%d selected"), NumItemsSelected));
	}

	return FText::GetEmpty();
}

FSlateColor SPjcTreeView::GetTreeOptionsBtnForegroundColor() const
{
	static const FName InvertedForegroundName("InvertedForeground");
	static const FName DefaultForegroundName("DefaultForeground");

	if (!TreeOptionBtn.IsValid()) return FEditorStyle::GetSlateColor(DefaultForegroundName);

	return TreeOptionBtn->IsHovered() ? FEditorStyle::GetSlateColor(InvertedForegroundName) : FEditorStyle::GetSlateColor(DefaultForegroundName);
}

void SPjcTreeView::UpdateMapInfo(TMap<FString, int32>& MapNum, TMap<FString, int64>& MapSize, const FString& AssetPath, int64 AssetSize)
{
	FString CurrentPath = AssetPath;

	// Iterate through all parent folders and update the asset count and size
	while (!CurrentPath.IsEmpty())
	{
		if (MapNum.Contains(CurrentPath))
		{
			MapNum[CurrentPath]++;
			MapSize[CurrentPath] += AssetSize;
		}
		else
		{
			MapNum.Add(CurrentPath, 1);
			MapSize.Add(CurrentPath, AssetSize);
		}

		// Remove the last folder in the path
		int32 LastSlashIndex;
		if (CurrentPath.FindLastChar('/', LastSlashIndex))
		{
			CurrentPath.LeftInline(LastSlashIndex, false);
		}
		else
		{
			CurrentPath.Empty();
		}
	}
}
