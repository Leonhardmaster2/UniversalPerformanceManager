// Copyright Universal Performance Manager. All Rights Reserved.

#include "UPMSettingsPanelWidget.h"
#include "GameFramework/GameUserSettings.h"

UUPMSettingsPanelWidget::UUPMSettingsPanelWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UUPMSettingsPanelWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Get the settings manager instance
    SettingsManager = UUPMSettingsManager::GetInstance(this);

    // Load settings and notify Blueprint
    if (SettingsManager)
    {
        RefreshFromSettings();
    }
}

// ==================== Initialization ====================

void UUPMSettingsPanelWidget::RefreshFromSettings()
{
    if (SettingsManager)
    {
        OnSettingsLoaded(SettingsManager->GetAllSettings());
    }
}

// ==================== Settings Access ====================

FUPMCompleteSettings UUPMSettingsPanelWidget::GetCurrentSettings() const
{
    if (SettingsManager)
    {
        return SettingsManager->GetAllSettings();
    }
    return FUPMCompleteSettings();
}

// ==================== Graphics Settings ====================

void UUPMSettingsPanelWidget::SetAntiAliasingQuality(int32 Quality)
{
    if (SettingsManager)
    {
        SettingsManager->SetAntiAliasingQuality(Quality);
    }
}

void UUPMSettingsPanelWidget::SetShadowQuality(int32 Quality)
{
    if (SettingsManager)
    {
        SettingsManager->SetShadowQuality(Quality);
    }
}

void UUPMSettingsPanelWidget::SetViewDistanceQuality(int32 Quality)
{
    if (SettingsManager)
    {
        SettingsManager->SetViewDistanceQuality(Quality);
    }
}

void UUPMSettingsPanelWidget::SetPostProcessQuality(int32 Quality)
{
    if (SettingsManager)
    {
        SettingsManager->SetPostProcessQuality(Quality);
    }
}

void UUPMSettingsPanelWidget::SetTextureQuality(int32 Quality)
{
    if (SettingsManager)
    {
        SettingsManager->SetTextureQuality(Quality);
    }
}

// ==================== Rendering Settings ====================

void UUPMSettingsPanelWidget::SetLumenEnabled(bool bEnabled)
{
    if (SettingsManager)
    {
        SettingsManager->SetLumenEnabled(bEnabled);
    }
}

void UUPMSettingsPanelWidget::SetRayTracingEnabled(bool bEnabled)
{
    if (SettingsManager)
    {
        SettingsManager->SetRayTracingEnabled(bEnabled);
    }
}

void UUPMSettingsPanelWidget::SetSSAOEnabled(bool bEnabled)
{
    if (SettingsManager)
    {
        SettingsManager->SetSSAOEnabled(bEnabled);
    }
}

void UUPMSettingsPanelWidget::SetSSREnabled(bool bEnabled)
{
    if (SettingsManager)
    {
        SettingsManager->SetSSREnabled(bEnabled);
    }
}

// ==================== Performance Settings ====================

void UUPMSettingsPanelWidget::SetVSyncEnabled(bool bEnabled)
{
    if (SettingsManager)
    {
        SettingsManager->SetVSyncEnabled(bEnabled);
    }
}

void UUPMSettingsPanelWidget::SetFrameRateLimit(float Limit)
{
    if (SettingsManager)
    {
        SettingsManager->SetFrameRateLimit(Limit);
    }
}

// ==================== Display Settings ====================

void UUPMSettingsPanelWidget::SetResolution(FIntPoint Resolution)
{
    if (SettingsManager)
    {
        SettingsManager->SetResolution(Resolution);
    }
}

void UUPMSettingsPanelWidget::SetWindowMode(TEnumAsByte<EWindowMode::Type> Mode)
{
    if (SettingsManager)
    {
        SettingsManager->SetWindowMode(Mode);
    }
}

TArray<FIntPoint> UUPMSettingsPanelWidget::GetAvailableResolutions() const
{
    TArray<FIntPoint> Resolutions;

    // UE 5.6+ compatible resolution retrieval
    UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings();
    if (GameSettings)
    {
        FScreenResolutionArray SupportedResolutions;
        if (RHIGetAvailableResolutions(SupportedResolutions, true))
        {
            for (const FScreenResolutionRHI& Resolution : SupportedResolutions)
            {
                Resolutions.AddUnique(FIntPoint(Resolution.Width, Resolution.Height));
            }
        }
    }

    // Add some common resolutions if none found
    if (Resolutions.Num() == 0)
    {
        Resolutions.Add(FIntPoint(1280, 720));
        Resolutions.Add(FIntPoint(1920, 1080));
        Resolutions.Add(FIntPoint(2560, 1440));
        Resolutions.Add(FIntPoint(3840, 2160));
    }

    return Resolutions;
}

// ==================== Audio Settings ====================

void UUPMSettingsPanelWidget::SetMasterVolume(float Volume)
{
    if (SettingsManager)
    {
        SettingsManager->SetMasterVolume(Volume);
    }
}

void UUPMSettingsPanelWidget::SetSFXVolume(float Volume)
{
    if (SettingsManager)
    {
        SettingsManager->SetSFXVolume(Volume);
    }
}

void UUPMSettingsPanelWidget::SetMusicVolume(float Volume)
{
    if (SettingsManager)
    {
        SettingsManager->SetMusicVolume(Volume);
    }
}

// ==================== Gameplay Settings ====================

void UUPMSettingsPanelWidget::SetFOV(float FOV)
{
    if (SettingsManager)
    {
        SettingsManager->SetFOV(FOV);
    }
}

void UUPMSettingsPanelWidget::SetMouseSensitivity(float Sensitivity)
{
    if (SettingsManager)
    {
        SettingsManager->SetMouseSensitivity(Sensitivity);
    }
}

// ==================== Save/Load ====================

bool UUPMSettingsPanelWidget::SaveSettings()
{
    if (SettingsManager)
    {
        return SettingsManager->SaveSettings();
    }
    return false;
}

bool UUPMSettingsPanelWidget::LoadSettings()
{
    if (SettingsManager)
    {
        bool bSuccess = SettingsManager->LoadSettings();
        if (bSuccess)
        {
            RefreshFromSettings();
        }
        return bSuccess;
    }
    return false;
}

void UUPMSettingsPanelWidget::ResetToDefaults()
{
    if (SettingsManager)
    {
        // Create default settings
        FUPMCompleteSettings DefaultSettings;

        // Apply defaults
        SettingsManager->SetGraphicsSettings(DefaultSettings.Graphics);
        SettingsManager->SetRenderingSettings(DefaultSettings.Rendering);
        SettingsManager->SetPerformanceSettings(DefaultSettings.Performance);
        SettingsManager->SetDisplaySettings(DefaultSettings.Display);
        SettingsManager->SetAudioSettings(DefaultSettings.Audio);
        SettingsManager->SetGameplaySettings(DefaultSettings.Gameplay);

        RefreshFromSettings();
    }
}

void UUPMSettingsPanelWidget::ApplySettings()
{
    if (SettingsManager)
    {
        SettingsManager->ApplyAllSettings();
    }
}
