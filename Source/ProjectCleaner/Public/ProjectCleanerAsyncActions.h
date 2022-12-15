﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "ProjectCleanerTypes.h"
#include "ProjectCleanerAsyncActions.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProjectCleanerOutputPin);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProjectCleanerOutputResult, const FProjectCleanerScanResult&, ScanResult);


UCLASS()
class UProjectCleanerScanAction final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category="ProjectCleanerScanner", meta=(ToolTip="Scans project for unused assets", BlueprintInternalUseOnly="true"))
	static UProjectCleanerScanAction* ScanProject(const FProjectCleanerScanSettings& ScanSettings);

private:
	UFUNCTION()
	void ExecuteScanProject();

public:
	UPROPERTY(BlueprintAssignable)
	FProjectCleanerOutputPin OnScanFailed;
	UPROPERTY(BlueprintAssignable)
	FProjectCleanerOutputPin OnScanFinished;
	UPROPERTY(BlueprintAssignable)
	FProjectCleanerOutputResult OutScanResult;

private:
	FProjectCleanerScanSettings ScanSettings;
	FProjectCleanerScanResult ScanResult;
};
