// Copyright Universal Performance Manager. All Rights Reserved.

#include "UniversalPerformanceManager.h"

#define LOCTEXT_NAMESPACE "FUniversalPerformanceManagerModule"

void FUniversalPerformanceManagerModule::StartupModule()
{
    // This code will execute after your module is loaded into memory
    UE_LOG(LogTemp, Log, TEXT("Universal Performance Manager module started"));
}

void FUniversalPerformanceManagerModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module
    UE_LOG(LogTemp, Log, TEXT("Universal Performance Manager module shutdown"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUniversalPerformanceManagerModule, UniversalPerformanceManager)
