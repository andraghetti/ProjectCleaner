﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectCleanerTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProjectCleanerLibrary.generated.h"

struct FProjectCleanerIndirectAsset;

UCLASS(DisplayName="ProjectCleanerLibrary", meta=(ToolTip="Project Cleaner collection of helper functions"))
class UProjectCleanerLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static void GetSubFolders(const FString& InDirPath, const bool bRecursive, TSet<FString>& SubFolders);
	static int32 GetSubFoldersNum(const FString& InDirPath, const bool bRecursive);
	static void GetEmptyFolders(const FString& InDirPath, TSet<FString>& EmptyFolders);
	static int32 GetEmptyFoldersNum(const FString& InDirPath);
	static bool IsEmptyFolder(const FString& InDirPath);
	static void GetNonEngineFiles(TSet<FString>& FilesNonEngine);

	static void GetAssetsUsed(TArray<FAssetData>& AssetsUsed);
	static void GetAssetsUnused(TArray<FAssetData>& AssetsUnused);
	static void GetAssetsPrimary(TArray<FAssetData>& AssetsPrimary, const bool bIncludeDerivedClasses = false);
	static void GetAssetsIndirect(TArray<FAssetData>& AssetsIndirect);
	static void GetAssetsCorrupted(TSet<FString>& FilesCorrupted);
	static void GetPrimaryAssetClasses(TArray<UClass*>& PrimaryAssetClasses, const bool bIncludeDerivedClasses = false);
	
	// UFUNCTION(BlueprintCallable, Category="ProjectCleaner", meta=(ToolTip="Returns all subdirectories for given directory. Specify Exclude Directories that you want to exclude"))
	// static void GetSubDirectories(const FString& RootDir, const bool bRecursive, TSet<FString>& SubDirectories, const TSet<FString>& ExcludeDirectories);
	//
	// UFUNCTION(BlueprintCallable, Category="ProjectCleaner", meta=(ToolTip="Returns all empty folders in given path"))
	// static void GetEmptyDirectories(const FString& InAbsPath, TSet<FString>& EmptyDirectories, const TSet<FString>& ExcludeDirectories);

	// UFUNCTION(BlueprintCallable, Category="ProjectCleaner", meta=(ToolTip="Returns all primary asset classes in project. This list is same as in AssetManager->Primary Asset Types To Scan"))
	// static void GetPrimaryAssetClasses(TArray<UClass*>& PrimaryAssetClasses);
	//
	// static void GetPrimaryAssetClassNames(TArray<FName>& ClassNames);
	//
	// UFUNCTION(BlueprintCallable, Category="ProjectCleaner", meta=(ToolTip="Returns all primary assets in project"))
	// static void GetPrimaryAssets(TArray<FAssetData>& PrimaryAssets);

	// UFUNCTION(BlueprintCallable, Category="ProjectCleaner", meta=(ToolTip="Returns all assets in directory. Specify ExcludeFilter for exclusion"))
	// static void GetAssetsInPath(const FString& InRelPath, const bool bRecursive, TArray<FAssetData>& Assets);

	UFUNCTION(BlueprintCallable, Category="ProjectCleaner", meta=(ToolTip="Returns total size of given assets array"))
	static int64 GetAssetsTotalSize(const TArray<FAssetData>& Assets);

	// UFUNCTION(BlueprintCallable, Category="ProjectCleaner", meta=(ToolTip="Returns all assets that used indirectly (source code, config files etc)"))
	// static void GetAssetsIndirect(TArray<FProjectCleanerIndirectAsset>& IndirectAssets);

	// UFUNCTION(BlueprintCallable, Category="ProjectCleaner",
	// 	meta=(ToolTip=
	// 		"Returns all corrupted files under Content directory. Corrupted files are engine .uasset or .umap files that for some reason failed to load and are not registered in AssetRegistry"))
	// static void GetAssetsCorrupted(TSet<FString>& CorruptedAssets);
	
	UFUNCTION(BlueprintCallable, Category="ProjectCleaner",
		meta=(ToolTip="Converts given relative to absolute. Example /Game/StarterContent => C:/{Your_Project_Path_To_Content_Folder}/StarterContent"))
	static FString PathConvertToAbs(const FString& InRelPath);

	UFUNCTION(BlueprintCallable, Category="ProjectCleaner",
		meta=(ToolTip="Converts given absolute to relative. Example C:/{Your_Project_Path_To_Content_Folder}/StarterContent => /Game/StarterContent"))
	static FString PathConvertToRel(const FString& InAbsPath);

	// UFUNCTION(BlueprintCallable, Category="ProjectCleaner", meta=(ToolTip="Returns all non engine files under Content directory"))
	// static void GetNonEngineFiles(TSet<FString>& NonEngineFiles);

	

	static FString GetAssetClassName(const FAssetData& AssetData);
	static void FixupRedirectors();
	static void SaveAllAssets(const bool bPromptUser = true);
	static void UpdateAssetRegistry(const bool bSyncScan = false);
	static void FocusOnDirectory(const FString& InRelPath);

	// UFUNCTION(BlueprintCallable, Category="ProjectCleaner", meta=(ToolTip="Checks if given extension is engine extension or not (.uasset or .umap)"))
	static bool IsEngineExtension(const FString& Extension);

	UFUNCTION(BlueprintCallable, Category="ProjectCleaner", meta=(ToolTip="Checks if given asset path is under megascans plugin folder or not"))
	static bool IsUnderMegascansFolder(const FString& AssetPackagePath);

	UFUNCTION(BlueprintCallable, Category="ProjectCleaner", meta=(ToolTip="Checks AssetRegistry is still working"))
	static bool IsAssetRegistryWorking();

	// static bool HasIndirectlyUsedAssets(const FString& FileContent);
private:
	static FString ConvertPathInternal(const FString& From, const FString& To, const FString& Path);
};
