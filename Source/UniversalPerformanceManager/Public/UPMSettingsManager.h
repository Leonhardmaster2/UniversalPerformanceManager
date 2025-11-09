// Copyright Universal Performance Manager. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/GameUserSettings.h"
#include "UPMSettingsManager.generated.h"

/**
 * Performance metrics data structure
 */
USTRUCT(BlueprintType)
struct FUPMPerformanceMetrics
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float FPS_Current;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float FPS_Average;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float FPS_Min;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float FPS_Max;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float CPUFrameTime;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float GPUFrameTime;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float VRAMUsageMB;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float RAMUsageMB;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    int32 DrawCalls;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    int32 PrimitiveCount;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float GameThreadLoad;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float RenderThreadLoad;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float RHIThreadLoad;

    FUPMPerformanceMetrics()
        : FPS_Current(0.0f)
        , FPS_Average(0.0f)
        , FPS_Min(0.0f)
        , FPS_Max(999.0f)
        , CPUFrameTime(0.0f)
        , GPUFrameTime(0.0f)
        , VRAMUsageMB(0.0f)
        , RAMUsageMB(0.0f)
        , DrawCalls(0)
        , PrimitiveCount(0)
        , GameThreadLoad(0.0f)
        , RenderThreadLoad(0.0f)
        , RHIThreadLoad(0.0f)
    {
    }
};

/**
 * Graphics settings structure
 */
USTRUCT(BlueprintType)
struct FUPMGraphicsSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Graphics")
    int32 AntiAliasingQuality;

    UPROPERTY(BlueprintReadWrite, Category = "Graphics")
    int32 ShadowQuality;

    UPROPERTY(BlueprintReadWrite, Category = "Graphics")
    int32 ViewDistanceQuality;

    UPROPERTY(BlueprintReadWrite, Category = "Graphics")
    int32 PostProcessQuality;

    UPROPERTY(BlueprintReadWrite, Category = "Graphics")
    int32 TextureQuality;

    UPROPERTY(BlueprintReadWrite, Category = "Graphics")
    int32 EffectsQuality;

    UPROPERTY(BlueprintReadWrite, Category = "Graphics")
    int32 FoliageQuality;

    UPROPERTY(BlueprintReadWrite, Category = "Graphics")
    int32 ShadingQuality;

    FUPMGraphicsSettings()
        : AntiAliasingQuality(3)
        , ShadowQuality(3)
        , ViewDistanceQuality(3)
        , PostProcessQuality(3)
        , TextureQuality(3)
        , EffectsQuality(3)
        , FoliageQuality(3)
        , ShadingQuality(3)
    {
    }
};

/**
 * Rendering settings structure
 */
USTRUCT(BlueprintType)
struct FUPMRenderingSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Rendering")
    bool bEnableLumen;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering")
    bool bEnableRayTracing;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering")
    bool bEnableSSAO;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering")
    bool bEnableSSR;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering")
    bool bEnableMotionBlur;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering")
    bool bEnableBloom;

    FUPMRenderingSettings()
        : bEnableLumen(true)
        , bEnableRayTracing(false)
        , bEnableSSAO(true)
        , bEnableSSR(true)
        , bEnableMotionBlur(true)
        , bEnableBloom(true)
    {
    }
};

/**
 * Performance settings structure
 */
USTRUCT(BlueprintType)
struct FUPMPerformanceSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Performance")
    bool bEnableVSync;

    UPROPERTY(BlueprintReadWrite, Category = "Performance")
    float FrameRateLimit;

    FUPMPerformanceSettings()
        : bEnableVSync(true)
        , FrameRateLimit(0.0f)
    {
    }
};

/**
 * Display settings structure
 */
USTRUCT(BlueprintType)
struct FUPMDisplaySettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Display")
    FIntPoint Resolution;

    UPROPERTY(BlueprintReadWrite, Category = "Display")
    TEnumAsByte<EWindowMode::Type> WindowMode;

    FUPMDisplaySettings()
        : Resolution(1920, 1080)
        , WindowMode(EWindowMode::Fullscreen)
    {
    }
};

/**
 * Audio settings structure
 */
USTRUCT(BlueprintType)
struct FUPMAudioSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Audio")
    float MasterVolume;

    UPROPERTY(BlueprintReadWrite, Category = "Audio")
    float SFXVolume;

    UPROPERTY(BlueprintReadWrite, Category = "Audio")
    float MusicVolume;

    FUPMAudioSettings()
        : MasterVolume(1.0f)
        , SFXVolume(1.0f)
        , MusicVolume(0.8f)
    {
    }
};

/**
 * Gameplay settings structure
 */
USTRUCT(BlueprintType)
struct FUPMGameplaySettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
    float FOV;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
    float MouseSensitivity;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
    bool bInvertMouseY;

    FUPMGameplaySettings()
        : FOV(90.0f)
        , MouseSensitivity(1.0f)
        , bInvertMouseY(false)
    {
    }
};

/**
 * Complete settings data structure
 */
USTRUCT(BlueprintType)
struct FUPMCompleteSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    FUPMGraphicsSettings Graphics;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    FUPMRenderingSettings Rendering;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    FUPMPerformanceSettings Performance;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    FUPMDisplaySettings Display;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    FUPMAudioSettings Audio;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    FUPMGameplaySettings Gameplay;
};

/**
 * Universal Performance Manager - Main settings and performance monitoring class
 *
 * This class provides:
 * - Real-time performance metrics monitoring
 * - Runtime settings management without game restart
 * - Persistent settings storage to Saved/UPM/Settings.json
 * - Works in packaged builds (Steam, itch.io, consoles)
 */
UCLASS(BlueprintType, Blueprintable)
class UNIVERSALPERFORMANCEMANAGER_API UUPMSettingsManager : public UObject
{
    GENERATED_BODY()

public:
    UUPMSettingsManager();

    // ==================== Singleton Access ====================

    /**
     * Get the global instance of the UPM Settings Manager
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Manager", meta = (WorldContext = "WorldContextObject"))
    static UUPMSettingsManager* GetInstance(const UObject* WorldContextObject);

    // ==================== Performance Monitoring ====================

    /**
     * Update performance metrics (call every frame or regularly)
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void UpdatePerformanceMetrics(float DeltaTime);

    /**
     * Get current performance metrics
     */
    UFUNCTION(BlueprintPure, Category = "UPM|Performance")
    FUPMPerformanceMetrics GetPerformanceMetrics() const { return PerformanceMetrics; }

    /**
     * Reset performance statistics (min, max, average)
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void ResetPerformanceStats();

    // ==================== Settings Management ====================

    /**
     * Get all current settings
     */
    UFUNCTION(BlueprintPure, Category = "UPM|Settings")
    FUPMCompleteSettings GetAllSettings() const { return CurrentSettings; }

    /**
     * Apply all settings immediately (without restart)
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Settings")
    void ApplyAllSettings();

    // ==================== Graphics Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Graphics")
    void SetGraphicsSettings(const FUPMGraphicsSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "UPM|Graphics")
    void SetAntiAliasingQuality(int32 Quality);

    UFUNCTION(BlueprintCallable, Category = "UPM|Graphics")
    void SetShadowQuality(int32 Quality);

    UFUNCTION(BlueprintCallable, Category = "UPM|Graphics")
    void SetViewDistanceQuality(int32 Quality);

    UFUNCTION(BlueprintCallable, Category = "UPM|Graphics")
    void SetPostProcessQuality(int32 Quality);

    UFUNCTION(BlueprintCallable, Category = "UPM|Graphics")
    void SetTextureQuality(int32 Quality);

    // ==================== Rendering Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetRenderingSettings(const FUPMRenderingSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetLumenEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetRayTracingEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetSSAOEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetSSREnabled(bool bEnabled);

    // ==================== Performance Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void SetPerformanceSettings(const FUPMPerformanceSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void SetVSyncEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void SetFrameRateLimit(float Limit);

    // ==================== Display Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetDisplaySettings(const FUPMDisplaySettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetResolution(FIntPoint Resolution);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetWindowMode(TEnumAsByte<EWindowMode::Type> Mode);

    // ==================== Audio Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetAudioSettings(const FUPMAudioSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetMasterVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetSFXVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetMusicVolume(float Volume);

    // ==================== Gameplay Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetGameplaySettings(const FUPMGameplaySettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetFOV(float FOV);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetMouseSensitivity(float Sensitivity);

    // ==================== Persistence ====================

    /**
     * Save all settings to Saved/UPM/Settings.json
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Persistence")
    bool SaveSettings();

    /**
     * Load settings from Saved/UPM/Settings.json
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Persistence")
    bool LoadSettings();

    /**
     * Auto-load settings on game start
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Persistence")
    void Initialize();

protected:
    // Current performance metrics
    UPROPERTY(BlueprintReadOnly, Category = "UPM|Performance")
    FUPMPerformanceMetrics PerformanceMetrics;

    // Current settings
    UPROPERTY(BlueprintReadOnly, Category = "UPM|Settings")
    FUPMCompleteSettings CurrentSettings;

private:
    // Performance tracking
    TArray<float> FPSHistory;
    float FPSHistoryTimeAccumulator;

    // Settings application
    void ApplyGraphicsSettings();
    void ApplyRenderingSettings();
    void ApplyPerformanceSettings();
    void ApplyDisplaySettings();
    void ApplyAudioSettings();
    void ApplyGameplaySettings();

    // Persistence helpers
    FString GetSettingsFilePath() const;
    TSharedPtr<FJsonObject> SettingsToJson() const;
    bool JsonToSettings(TSharedPtr<FJsonObject> JsonObject);

    // Singleton instance
    static UUPMSettingsManager* Instance;
};
