// Copyright Universal Performance Manager. All Rights Reserved.

#include "UPMPerformanceOverlayWidget.h"

UUPMPerformanceOverlayWidget::UUPMPerformanceOverlayWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , bIsVisible(true)
    , UpdateInterval(0.1f) // Update 10 times per second by default
    , UpdateTimer(0.0f)
{
}

void UUPMPerformanceOverlayWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Get the settings manager instance
    SettingsManager = UUPMSettingsManager::GetInstance(this);
}

void UUPMPerformanceOverlayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!SettingsManager || !bIsVisible)
    {
        return;
    }

    // Update performance metrics
    SettingsManager->UpdatePerformanceMetrics(InDeltaTime);

    // Update UI at specified interval
    UpdateTimer += InDeltaTime;
    if (UpdateTimer >= UpdateInterval)
    {
        UpdateTimer = 0.0f;
        OnPerformanceMetricsUpdated(SettingsManager->GetPerformanceMetrics());
    }
}

FUPMPerformanceMetrics UUPMPerformanceOverlayWidget::GetPerformanceMetrics() const
{
    if (SettingsManager)
    {
        return SettingsManager->GetPerformanceMetrics();
    }
    return FUPMPerformanceMetrics();
}

void UUPMPerformanceOverlayWidget::ToggleOverlay()
{
    SetOverlayVisible(!bIsVisible);
}

void UUPMPerformanceOverlayWidget::SetOverlayVisible(bool bVisible)
{
    bIsVisible = bVisible;
    SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UUPMPerformanceOverlayWidget::ResetStats()
{
    if (SettingsManager)
    {
        SettingsManager->ResetPerformanceStats();
    }
}
