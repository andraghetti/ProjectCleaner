﻿// Copyright 2021. Ashot Barkhudaryan. All Rights Reserved.

#include "UI/ProjectCleanerCorruptedFilesUI.h"
#include "Core/ProjectCleanerUtility.h"
#include "UI/ProjectCleanerStyle.h"
// Engine Headers
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "FProjectCleanerModule"

void SProjectCleanerCorruptedFilesUI::Construct(const FArguments& InArgs)
{
	if (InArgs._CorruptedFiles)
	{
		SetCorruptedFiles(*InArgs._CorruptedFiles);
	}

	InitUI();
}

void SProjectCleanerCorruptedFilesUI::SetCorruptedFiles(const TSet<FString>& NewCorruptedFiles)
{
	CorruptedFiles.Reset();
	CorruptedFiles.Reserve(NewCorruptedFiles.Num());

	for (const auto File : NewCorruptedFiles)
	{
		const auto& CorruptedFile = NewObject<UCorruptedFile>();
		CorruptedFile->Name = FPaths::GetBaseFilename(File);
		CorruptedFile->AbsolutePath = File;
		CorruptedFiles.AddUnique(CorruptedFile);
	}

	if (ListView.IsValid())
	{
		ListView->SetListItemsSource(CorruptedFiles);
		ListView->RebuildList();
	}
}

void SProjectCleanerCorruptedFilesUI::InitUI()
{
	ListView = SNew(SListView<TWeakObjectPtr<UCorruptedFile>>)
		.ListItemsSource(&CorruptedFiles)
		.SelectionMode(ESelectionMode::SingleToggle)
		.OnGenerateRow(this, &SProjectCleanerCorruptedFilesUI::OnGenerateRow)
		.OnMouseButtonDoubleClick_Raw(this, &SProjectCleanerCorruptedFilesUI::OnMouseDoubleClick)
		.HeaderRow
		(
			SNew(SHeaderRow)
			+ SHeaderRow::Column(FName("Name"))
			.HAlignCell(HAlign_Center)
			.VAlignCell(VAlign_Center)
			.HAlignHeader(HAlign_Center)
			.HeaderContentPadding(FMargin(10.0f))
			.FillWidth(0.3f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("NameColumn", "Name"))
			]
			+ SHeaderRow::Column(FName("AbsolutePath"))
			.HAlignCell(HAlign_Center)
			.VAlignCell(VAlign_Center)
			.HAlignHeader(HAlign_Center)
			.HeaderContentPadding(FMargin(10.0f))
			.FillWidth(0.7f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("PathColumn", "FilePath"))
			]
	);

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.Padding(20.0f)
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SVerticalBox)
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.AutoWrapText(true)
						.Font(FProjectCleanerStyle::Get().GetFontStyle("ProjectCleaner.Font.Light20"))
						.Text(LOCTEXT("corrupted_files", "Corrupted Files"))
					]
					//+SVerticalBox::Slot()
					//.AutoHeight()
					//.Padding(FMargin{0.0f, 10.0f})
					//[
					//	SNew(STextBlock)
					//	.AutoWrapText(true)
					//	.Font(FProjectCleanerStyle::Get().GetFontStyle("ProjectCleaner.Font.Light15"))
					//	.Text(LOCTEXT("corrupted_files_tip_text", ""))
					//]
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(FMargin{0.0f, 10.0f})
					[
						SNew(STextBlock)
						.AutoWrapText(true)
						.Font(FProjectCleanerStyle::Get().GetFontStyle("ProjectCleaner.Font.Light10"))
						.Text(LOCTEXT("corrupted_files_fix_text", "How to fix?:\n\t1.Close Editor\n\t2.Delete files manually from Windows explorer"))
					]
					+ SVerticalBox::Slot()
					.Padding(FMargin{ 0.0f, 10.0f })
					.AutoHeight()
					[
						SNew(STextBlock)
						.AutoWrapText(true)
						.Font(FProjectCleanerStyle::Get().GetFontStyle("ProjectCleaner.Font.Light10"))
						.Text(LOCTEXT("corrupted_files_dbl_click_on_row", "Double click on row to open in Explorer"))
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin{ 0.0f, 20.0f })
				[
					ListView.ToSharedRef()
				]
			]
		]
	];
}

TSharedRef<ITableRow> SProjectCleanerCorruptedFilesUI::OnGenerateRow(TWeakObjectPtr<UCorruptedFile> InItem,
	const TSharedRef<STableViewBase>& OwnerTable) const
{
	return SNew(SCorruptedFileUISelectionRow, OwnerTable).SelectedRowItem(InItem);
}

void SProjectCleanerCorruptedFilesUI::OnMouseDoubleClick(TWeakObjectPtr<UCorruptedFile> Item) const
{
	if (!Item.IsValid()) return;

	const auto DirectoryPath = FPaths::GetPath(Item.Get()->AbsolutePath);
	if (!FPaths::DirectoryExists(DirectoryPath)) return;
	
	FPlatformProcess::ExploreFolder(*DirectoryPath);
}

#undef LOCTEXT_NAMESPACE