﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProjectCleanerLibPath.generated.h"

UCLASS()
class UProjectCleanerLibPath final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ProjectCleaner|Lib|Path", meta=(ToolTip="Normalized given file or directory path."))
	static FString Normalize(const FString& InPath);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ProjectCleaner|Lib|Path", meta=(ToolTip="Converts given to absolute path. If path is outside Content folder will return empty string."))
	static FString ConvertToAbs(const FString& InPath);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ProjectCleaner|Lib|Path", meta=(ToolTip="Converts given to relative path. If path is outside Content folder will return empty string."))
	static FString ConvertToRel(const FString& InPath);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ProjectCleaner|Lib|Path", meta=(ToolTip="Returns full path to project Content folder"))
	static FString GetFolderContent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ProjectCleaner|Lib|Path", meta=(ToolTip="Returns full path to project Developers folder"))
	static FString GetFolderDevelopers();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ProjectCleaner|Lib|Path", meta=(ToolTip="Returns full path to project Collections folder"))
	static FString GetFolderCollections();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ProjectCleaner|Lib|Path", meta=(ToolTip="Returns full path to Developers folder of current user"))
	static FString GetFolderDevelopersUser();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ProjectCleaner|Lib|Path", meta=(ToolTip="Returns full path to Collections folder of current user"))
	static FString GetFolderCollectionsUser();

private:
	static bool PathIsUnderContentFolder(const FString& InPath);
};
