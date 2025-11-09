// Copyright Universal Performance Manager. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPMSettingsManager.h"
#include "UPMSettingsPanelWidget.generated.h"

/**
 * Base class for Settings Panel widget
 * Create a Blueprint child of this class and design your own settings UI
 *
 * This widget provides easy access to all UPM settings with automatic
 * save/load functionality
 */
UCLASS(BlueprintType, Blueprintable)
class UNIVERSALPERFORMANCEMANAGER_API UUPMSettingsPanelWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UUPMSettingsPanelWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;

    // ==================== Initialization ====================

    /**
     * Called when settings are loaded - use to initialize UI controls
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "UPM|Settings Panel")
    void OnSettingsLoaded(const FUPMCompleteSettings& Settings);

    /**
     * Refresh UI from current settings
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel")
    void RefreshFromSettings();

    // ==================== Settings Access ====================

    /**
     * Get all current settings
     */
    UFUNCTION(BlueprintPure, Category = "UPM|Settings Panel")
    FUPMCompleteSettings GetCurrentSettings() const;

    // ==================== Graphics Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Graphics")
    void SetAntiAliasingQuality(int32 Quality);

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Graphics")
    void SetShadowQuality(int32 Quality);

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Graphics")
    void SetViewDistanceQuality(int32 Quality);

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Graphics")
    void SetPostProcessQuality(int32 Quality);

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Graphics")
    void SetTextureQuality(int32 Quality);

    // ==================== Rendering Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Rendering")
    void SetLumenEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Rendering")
    void SetRayTracingEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Rendering")
    void SetSSAOEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Rendering")
    void SetSSREnabled(bool bEnabled);

    // ==================== Performance Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Performance")
    void SetVSyncEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Performance")
    void SetFrameRateLimit(float Limit);

    // ==================== Display Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Display")
    void SetResolution(FIntPoint Resolution);

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Display")
    void SetWindowMode(TEnumAsByte<EWindowMode::Type> Mode);

    /**
     * Get available screen resolutions
     */
    UFUNCTION(BlueprintPure, Category = "UPM|Settings Panel|Display")
    TArray<FIntPoint> GetAvailableResolutions() const;

    // ==================== Audio Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Audio")
    void SetMasterVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Audio")
    void SetSFXVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Audio")
    void SetMusicVolume(float Volume);

    // ==================== Gameplay Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Gameplay")
    void SetFOV(float FOV);

    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel|Gameplay")
    void SetMouseSensitivity(float Sensitivity);

    // ==================== Save/Load ====================

    /**
     * Save all current settings to disk
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel")
    bool SaveSettings();

    /**
     * Load settings from disk
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel")
    bool LoadSettings();

    /**
     * Reset all settings to defaults
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel")
    void ResetToDefaults();

    /**
     * Apply all settings immediately
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Settings Panel")
    void ApplySettings();

protected:
    UPROPERTY(BlueprintReadOnly, Category = "UPM|Settings Panel")
    UUPMSettingsManager* SettingsManager;
};
