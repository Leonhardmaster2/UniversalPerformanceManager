// Copyright Universal Performance Manager. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPMSettingsManager.h"
#include "UPMPerformanceOverlayWidget.generated.h"

/**
 * Base class for Performance Overlay widget
 * Create a Blueprint child of this class and design your own overlay UI
 *
 * This widget automatically updates performance metrics and provides
 * Blueprint-accessible functions to display them
 */
UCLASS(BlueprintType, Blueprintable)
class UNIVERSALPERFORMANCEMANAGER_API UUPMPerformanceOverlayWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UUPMPerformanceOverlayWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    /**
     * Override this in Blueprint to update your UI with new performance data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "UPM|Performance Overlay")
    void OnPerformanceMetricsUpdated(const FUPMPerformanceMetrics& Metrics);

    /**
     * Get the current performance metrics
     */
    UFUNCTION(BlueprintPure, Category = "UPM|Performance Overlay")
    FUPMPerformanceMetrics GetPerformanceMetrics() const;

    /**
     * Toggle overlay visibility
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Performance Overlay")
    void ToggleOverlay();

    /**
     * Set overlay visibility
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Performance Overlay")
    void SetOverlayVisible(bool bVisible);

    /**
     * Check if overlay is visible
     */
    UFUNCTION(BlueprintPure, Category = "UPM|Performance Overlay")
    bool IsOverlayVisible() const { return bIsVisible; }

    /**
     * Reset performance statistics
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Performance Overlay")
    void ResetStats();

protected:
    UPROPERTY(BlueprintReadOnly, Category = "UPM|Performance Overlay")
    UUPMSettingsManager* SettingsManager;

    UPROPERTY(BlueprintReadWrite, Category = "UPM|Performance Overlay")
    bool bIsVisible;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UPM|Performance Overlay")
    float UpdateInterval;

private:
    float UpdateTimer;
};
