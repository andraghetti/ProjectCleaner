﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PjcTypes.generated.h"

UENUM(BlueprintType)
enum class EPjcCleanupMethod : uint8
{
	None UMETA(Hidden),
	Full UMETA(ToolTip="Remove both unused assets and empty folders"),
	UnusedAssetsOnly UMETA(Tooltip="Remove unused assets only"),
	EmptyFoldersOnly UMETA(Tooltip="Remove empty folders only"),
};

USTRUCT(BlueprintType)
struct FPjcAssetExcludeSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AssetExcludeSettings", meta=(ToolTip="Consider assets in specified path as used"))
	TArray<FName> ExcludedPackagePaths;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AssetExcludeSettings", meta=(ToolTip="Consider specified assets as used."))
	TArray<FName> ExcludedObjectPaths;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AssetExcludeSettings", meta=(ToolTip="Consider assets of specified classes as used."))
	TArray<FName> ExcludedClassNames;
};

USTRUCT(BlueprintType)
struct FPjcFileInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FileInfo")
	int32 FileNum = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FileInfo")
	FString FilePath;

	bool operator==(const FPjcFileInfo& Other) const
	{
		return FilePath.Equals(Other.FilePath) && FileNum == Other.FileNum;
	}

	bool operator!=(const FPjcFileInfo& Other) const
	{
		return !(FilePath.Equals(Other.FilePath) && FileNum == Other.FileNum);
	}
};

USTRUCT(BlueprintType)
struct FPjcAssetIndirectUsageInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Asset Indirect Usage Info")
	TArray<FPjcFileInfo> Files;
};
