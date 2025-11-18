// Copyright Universal Performance Manager. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/GameUserSettings.h"
#include "UPMSettingsManager.generated.h"

/**
 * Colorblind mode enumeration
 */
UENUM(BlueprintType)
enum class EUPMColorblindMode : uint8
{
    None UMETA(DisplayName = "None"),
    Deuteranopia UMETA(DisplayName = "Deuteranopia (Green-Blind)"),
    Protanopia UMETA(DisplayName = "Protanopia (Red-Blind)"),
    Tritanopia UMETA(DisplayName = "Tritanopia (Blue-Blind)")
};

/**
 * Upscaling technology enumeration
 */
UENUM(BlueprintType)
enum class EUPMUpscalingMode : uint8
{
    None UMETA(DisplayName = "None"),
    DLSS UMETA(DisplayName = "NVIDIA DLSS"),
    FSR UMETA(DisplayName = "AMD FSR"),
    XeSS UMETA(DisplayName = "Intel XeSS"),
    TSR UMETA(DisplayName = "Temporal Super Resolution")
};

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

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float NetworkPing;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float PacketLoss;

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
        , NetworkPing(0.0f)
        , PacketLoss(0.0f)
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
 * Rendering settings structure - EXPANDED with advanced effects
 */
USTRUCT(BlueprintType)
struct FUPMRenderingSettings
{
    GENERATED_BODY()

    // Original settings
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

    // NEW: Advanced rendering features
    UPROPERTY(BlueprintReadWrite, Category = "Rendering|PostProcess")
    bool bEnableDepthOfField;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering|PostProcess")
    bool bEnableLensFlares;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering|PostProcess")
    bool bEnableChromaticAberration;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering|PostProcess")
    bool bEnableFilmGrain;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering|PostProcess")
    bool bEnableVignette;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering|Quality")
    bool bEnableVolumetricFog;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering|Quality")
    int32 AnisotropicFiltering; // 0=Off, 1=2x, 2=4x, 3=8x, 4=16x

    UPROPERTY(BlueprintReadWrite, Category = "Rendering|Quality")
    bool bEnableTAA;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering|Quality")
    EUPMUpscalingMode UpscalingMode;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering|Quality")
    int32 GlobalIlluminationQuality; // 0-4

    UPROPERTY(BlueprintReadWrite, Category = "Rendering|Quality")
    int32 ReflectionQuality; // 0-4

    UPROPERTY(BlueprintReadWrite, Category = "Rendering|Quality")
    bool bEnableSSGI;

    UPROPERTY(BlueprintReadWrite, Category = "Rendering|Quality")
    bool bEnableContactShadows;

    FUPMRenderingSettings()
        : bEnableLumen(true)
        , bEnableRayTracing(false)
        , bEnableSSAO(true)
        , bEnableSSR(true)
        , bEnableMotionBlur(true)
        , bEnableBloom(true)
        , bEnableDepthOfField(true)
        , bEnableLensFlares(true)
        , bEnableChromaticAberration(false)
        , bEnableFilmGrain(false)
        , bEnableVignette(true)
        , bEnableVolumetricFog(true)
        , AnisotropicFiltering(4)
        , bEnableTAA(true)
        , UpscalingMode(EUPMUpscalingMode::TSR)
        , GlobalIlluminationQuality(3)
        , ReflectionQuality(3)
        , bEnableSSGI(false)
        , bEnableContactShadows(true)
    {
    }
};

/**
 * Performance settings structure - EXPANDED
 */
USTRUCT(BlueprintType)
struct FUPMPerformanceSettings
{
    GENERATED_BODY()

    // Original settings
    UPROPERTY(BlueprintReadWrite, Category = "Performance")
    bool bEnableVSync;

    UPROPERTY(BlueprintReadWrite, Category = "Performance")
    float FrameRateLimit;

    // NEW: Advanced performance features
    UPROPERTY(BlueprintReadWrite, Category = "Performance|Dynamic")
    bool bEnableDynamicResolution;

    UPROPERTY(BlueprintReadWrite, Category = "Performance|Dynamic")
    float MinFrameRateForDynamicRes;

    UPROPERTY(BlueprintReadWrite, Category = "Performance|Advanced")
    bool bEnableTripleBuffering;

    UPROPERTY(BlueprintReadWrite, Category = "Performance|Advanced")
    bool bEnableAsyncCompute;

    UPROPERTY(BlueprintReadWrite, Category = "Performance|Advanced")
    float LODDistanceMultiplier; // 0.5 = closer LODs, 2.0 = farther LODs

    UPROPERTY(BlueprintReadWrite, Category = "Performance|Advanced")
    int32 ProcessPriority; // 0=Normal, 1=High, 2=RealTime

    FUPMPerformanceSettings()
        : bEnableVSync(true)
        , FrameRateLimit(0.0f)
        , bEnableDynamicResolution(false)
        , MinFrameRateForDynamicRes(30.0f)
        , bEnableTripleBuffering(false)
        , bEnableAsyncCompute(true)
        , LODDistanceMultiplier(1.0f)
        , ProcessPriority(0)
    {
    }
};

/**
 * Display settings structure - EXPANDED
 */
USTRUCT(BlueprintType)
struct FUPMDisplaySettings
{
    GENERATED_BODY()

    // Original settings
    UPROPERTY(BlueprintReadWrite, Category = "Display")
    FIntPoint Resolution;

    UPROPERTY(BlueprintReadWrite, Category = "Display")
    TEnumAsByte<EWindowMode::Type> WindowMode;

    // NEW: Advanced display features
    UPROPERTY(BlueprintReadWrite, Category = "Display|Visual")
    float Brightness; // 0.0 - 2.0, 1.0 = default

    UPROPERTY(BlueprintReadWrite, Category = "Display|Visual")
    float Contrast; // 0.0 - 2.0, 1.0 = default

    UPROPERTY(BlueprintReadWrite, Category = "Display|HDR")
    bool bEnableHDR;

    UPROPERTY(BlueprintReadWrite, Category = "Display|HDR")
    float HDRMaxNits; // 1000-10000

    UPROPERTY(BlueprintReadWrite, Category = "Display|Monitor")
    int32 MonitorIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Display|Window")
    bool bBorderlessWindow;

    UPROPERTY(BlueprintReadWrite, Category = "Display|Scaling")
    float ScreenPercentage; // 50-200, 100 = native

    UPROPERTY(BlueprintReadWrite, Category = "Display|UI")
    float MenuFieldOfView; // For menus/UI camera

    UPROPERTY(BlueprintReadWrite, Category = "Display|Advanced")
    float AspectRatioOverride; // 0 = auto, e.g., 1.777 for 16:9

    UPROPERTY(BlueprintReadWrite, Category = "Display|SafeZone")
    float SafeZoneScale; // 0.9 - 1.0 for console TVs

    FUPMDisplaySettings()
        : Resolution(1920, 1080)
        , WindowMode(EWindowMode::Fullscreen)
        , Brightness(1.0f)
        , Contrast(1.0f)
        , bEnableHDR(false)
        , HDRMaxNits(1000.0f)
        , MonitorIndex(0)
        , bBorderlessWindow(false)
        , ScreenPercentage(100.0f)
        , MenuFieldOfView(90.0f)
        , AspectRatioOverride(0.0f)
        , SafeZoneScale(1.0f)
    {
    }
};

/**
 * Audio settings structure - EXPANDED
 */
USTRUCT(BlueprintType)
struct FUPMAudioSettings
{
    GENERATED_BODY()

    // Original settings
    UPROPERTY(BlueprintReadWrite, Category = "Audio|Volume")
    float MasterVolume;

    UPROPERTY(BlueprintReadWrite, Category = "Audio|Volume")
    float SFXVolume;

    UPROPERTY(BlueprintReadWrite, Category = "Audio|Volume")
    float MusicVolume;

    // NEW: Expanded audio controls
    UPROPERTY(BlueprintReadWrite, Category = "Audio|Volume")
    float VoiceDialogVolume;

    UPROPERTY(BlueprintReadWrite, Category = "Audio|Volume")
    float AmbientVolume;

    UPROPERTY(BlueprintReadWrite, Category = "Audio|Volume")
    float UISoundVolume;

    UPROPERTY(BlueprintReadWrite, Category = "Audio|Volume")
    float VoiceChatVolume;

    UPROPERTY(BlueprintReadWrite, Category = "Audio|Quality")
    int32 AudioQuality; // 0=Low, 1=Medium, 2=High, 3=Ultra

    UPROPERTY(BlueprintReadWrite, Category = "Audio|Surround")
    int32 SurroundSoundMode; // 0=Stereo, 1=5.1, 2=7.1

    UPROPERTY(BlueprintReadWrite, Category = "Audio|Spatial")
    bool bEnableSpatialAudio;

    UPROPERTY(BlueprintReadWrite, Category = "Audio|Advanced")
    float DynamicRange; // 0.0-1.0, compression level

    UPROPERTY(BlueprintReadWrite, Category = "Audio|Subtitles")
    float SubtitleTextSize; // 0.5 - 2.0

    UPROPERTY(BlueprintReadWrite, Category = "Audio|Subtitles")
    float SubtitleBackgroundOpacity; // 0.0 - 1.0

    FUPMAudioSettings()
        : MasterVolume(1.0f)
        , SFXVolume(1.0f)
        , MusicVolume(0.8f)
        , VoiceDialogVolume(1.0f)
        , AmbientVolume(0.7f)
        , UISoundVolume(0.9f)
        , VoiceChatVolume(1.0f)
        , AudioQuality(2)
        , SurroundSoundMode(0)
        , bEnableSpatialAudio(false)
        , DynamicRange(0.5f)
        , SubtitleTextSize(1.0f)
        , SubtitleBackgroundOpacity(0.5f)
    {
    }
};

/**
 * Gameplay settings structure - EXPANDED
 */
USTRUCT(BlueprintType)
struct FUPMGameplaySettings
{
    GENERATED_BODY()

    // Original settings
    UPROPERTY(BlueprintReadWrite, Category = "Gameplay|Camera")
    float FOV;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay|Input")
    float MouseSensitivity;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay|Input")
    bool bInvertMouseY;

    // NEW: Controller and input settings
    UPROPERTY(BlueprintReadWrite, Category = "Gameplay|Controller")
    float ControllerSensitivity;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay|Controller")
    float ControllerDeadZone; // 0.0 - 0.5

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay|Aim")
    float AimAssistStrength; // 0.0 - 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay|Camera")
    float CameraShakeIntensity; // 0.0 - 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay|Camera")
    float HeadBobIntensity; // 0.0 - 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay|Controller")
    bool bEnableVibration;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay|Controls")
    bool bCrouchToggle; // true=toggle, false=hold

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay|Controls")
    bool bSprintToggle; // true=toggle, false=hold

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay|Controls")
    bool bEnableAutoRun;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay|Camera")
    float CameraSmoothing; // 0.0 - 1.0

    FUPMGameplaySettings()
        : FOV(90.0f)
        , MouseSensitivity(1.0f)
        , bInvertMouseY(false)
        , ControllerSensitivity(1.0f)
        , ControllerDeadZone(0.15f)
        , AimAssistStrength(0.5f)
        , CameraShakeIntensity(1.0f)
        , HeadBobIntensity(0.5f)
        , bEnableVibration(true)
        , bCrouchToggle(false)
        , bSprintToggle(false)
        , bEnableAutoRun(false)
        , CameraSmoothing(0.5f)
    {
    }
};

/**
 * NEW: Accessibility settings structure
 */
USTRUCT(BlueprintType)
struct FUPMAccessibilitySettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Accessibility|Vision")
    EUPMColorblindMode ColorblindMode;

    UPROPERTY(BlueprintReadWrite, Category = "Accessibility|UI")
    float UIScale; // 0.5 - 2.0

    UPROPERTY(BlueprintReadWrite, Category = "Accessibility|UI")
    float TextSize; // 0.5 - 2.0

    UPROPERTY(BlueprintReadWrite, Category = "Accessibility|Vision")
    bool bHighContrastMode;

    UPROPERTY(BlueprintReadWrite, Category = "Accessibility|Audio")
    bool bEnableScreenReader;

    UPROPERTY(BlueprintReadWrite, Category = "Accessibility|Motion")
    bool bReducedMotion;

    UPROPERTY(BlueprintReadWrite, Category = "Accessibility|Vision")
    bool bPhotosensitivityMode;

    FUPMAccessibilitySettings()
        : ColorblindMode(EUPMColorblindMode::None)
        , UIScale(1.0f)
        , TextSize(1.0f)
        , bHighContrastMode(false)
        , bEnableScreenReader(false)
        , bReducedMotion(false)
        , bPhotosensitivityMode(false)
    {
    }
};

/**
 * NEW: Network settings structure
 */
USTRUCT(BlueprintType)
struct FUPMNetworkSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Network")
    int32 MaxPingThreshold; // Max acceptable ping in ms

    UPROPERTY(BlueprintReadWrite, Category = "Network")
    float NetworkSmoothing; // 0.0 - 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Network")
    int32 BandwidthLimitKBps; // 0 = unlimited

    UPROPERTY(BlueprintReadWrite, Category = "Network")
    FString PreferredRegion; // e.g., "US-East", "EU-West"

    UPROPERTY(BlueprintReadWrite, Category = "Network")
    bool bEnableCrossplay;

    FUPMNetworkSettings()
        : MaxPingThreshold(150)
        , NetworkSmoothing(0.5f)
        , BandwidthLimitKBps(0)
        , PreferredRegion(TEXT("Auto"))
        , bEnableCrossplay(true)
    {
    }
};

/**
 * NEW: Debug and advanced settings structure
 */
USTRUCT(BlueprintType)
struct FUPMDebugSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Debug")
    bool bShowPerformanceOverlay;

    UPROPERTY(BlueprintReadWrite, Category = "Debug")
    bool bShowNetworkStats;

    UPROPERTY(BlueprintReadWrite, Category = "Debug")
    bool bDeveloperMode;

    UPROPERTY(BlueprintReadWrite, Category = "Debug")
    bool bEnableCrashReporting;

    UPROPERTY(BlueprintReadWrite, Category = "Debug")
    bool bBenchmarkMode;

    FUPMDebugSettings()
        : bShowPerformanceOverlay(false)
        , bShowNetworkStats(false)
        , bDeveloperMode(false)
        , bEnableCrashReporting(true)
        , bBenchmarkMode(false)
    {
    }
};

/**
 * Complete settings data structure - EXPANDED with all new categories
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

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    FUPMAccessibilitySettings Accessibility;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    FUPMNetworkSettings Network;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    FUPMDebugSettings Debug;
};

/**
 * Universal Performance Manager - Main settings and performance monitoring class
 * EXPANDED with comprehensive settings support
 *
 * This class provides:
 * - Real-time performance metrics monitoring
 * - Runtime settings management without game restart
 * - Persistent settings storage to Saved/UPM/Settings.json
 * - Works in packaged builds (Steam, itch.io, consoles)
 * - Complete accessibility support
 * - Network configuration
 * - Debug and advanced options
 */
UCLASS(BlueprintType, Blueprintable)
class UNIVERSALPERFORMANCEMANAGER_API UUPMSettingsManager : public UObject
{
    GENERATED_BODY()

public:
    UUPMSettingsManager();

    // ==================== Singleton Access ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Manager", meta = (WorldContext = "WorldContextObject"))
    static UUPMSettingsManager* GetInstance(const UObject* WorldContextObject);

    // ==================== Performance Monitoring ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void UpdatePerformanceMetrics(float DeltaTime);

    UFUNCTION(BlueprintPure, Category = "UPM|Performance")
    FUPMPerformanceMetrics GetPerformanceMetrics() const { return PerformanceMetrics; }

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void ResetPerformanceStats();

    // ==================== Settings Management ====================

    UFUNCTION(BlueprintPure, Category = "UPM|Settings")
    FUPMCompleteSettings GetAllSettings() const { return CurrentSettings; }

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

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetDepthOfFieldEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetLensFlaresEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetChromaticAberrationEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetFilmGrainEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetVignetteEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetVolumetricFogEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetAnisotropicFiltering(int32 Level);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetTAAEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetUpscalingMode(EUPMUpscalingMode Mode);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetGlobalIlluminationQuality(int32 Quality);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetReflectionQuality(int32 Quality);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetSSGIEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Rendering")
    void SetContactShadowsEnabled(bool bEnabled);

    // ==================== Performance Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void SetPerformanceSettings(const FUPMPerformanceSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void SetVSyncEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void SetFrameRateLimit(float Limit);

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void SetDynamicResolutionEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void SetMinFrameRateForDynamicRes(float MinFPS);

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void SetTripleBufferingEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void SetAsyncComputeEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void SetLODDistanceMultiplier(float Multiplier);

    UFUNCTION(BlueprintCallable, Category = "UPM|Performance")
    void SetProcessPriority(int32 Priority);

    // ==================== Display Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetDisplaySettings(const FUPMDisplaySettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetResolution(FIntPoint Resolution);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetWindowMode(TEnumAsByte<EWindowMode::Type> Mode);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetBrightness(float Brightness);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetContrast(float Contrast);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetHDREnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetHDRMaxNits(float MaxNits);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetMonitorIndex(int32 Index);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetBorderlessWindow(bool bBorderless);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetScreenPercentage(float Percentage);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetMenuFieldOfView(float FOV);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetAspectRatioOverride(float AspectRatio);

    UFUNCTION(BlueprintCallable, Category = "UPM|Display")
    void SetSafeZoneScale(float Scale);

    // ==================== Audio Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetAudioSettings(const FUPMAudioSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetMasterVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetSFXVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetMusicVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetVoiceDialogVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetAmbientVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetUISoundVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetVoiceChatVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetAudioQuality(int32 Quality);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetSurroundSoundMode(int32 Mode);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetSpatialAudioEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetDynamicRange(float Range);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetSubtitleTextSize(float Size);

    UFUNCTION(BlueprintCallable, Category = "UPM|Audio")
    void SetSubtitleBackgroundOpacity(float Opacity);

    // ==================== Gameplay Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetGameplaySettings(const FUPMGameplaySettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetFOV(float FOV);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetMouseSensitivity(float Sensitivity);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetControllerSensitivity(float Sensitivity);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetControllerDeadZone(float DeadZone);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetAimAssistStrength(float Strength);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetCameraShakeIntensity(float Intensity);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetHeadBobIntensity(float Intensity);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetVibrationEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetCrouchToggle(bool bToggle);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetSprintToggle(bool bToggle);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetAutoRunEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Gameplay")
    void SetCameraSmoothing(float Smoothing);

    // ==================== Accessibility Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Accessibility")
    void SetAccessibilitySettings(const FUPMAccessibilitySettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "UPM|Accessibility")
    void SetColorblindMode(EUPMColorblindMode Mode);

    UFUNCTION(BlueprintCallable, Category = "UPM|Accessibility")
    void SetUIScale(float Scale);

    UFUNCTION(BlueprintCallable, Category = "UPM|Accessibility")
    void SetTextSize(float Size);

    UFUNCTION(BlueprintCallable, Category = "UPM|Accessibility")
    void SetHighContrastMode(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Accessibility")
    void SetScreenReaderEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Accessibility")
    void SetReducedMotion(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Accessibility")
    void SetPhotosensitivityMode(bool bEnabled);

    // ==================== Network Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Network")
    void SetNetworkSettings(const FUPMNetworkSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "UPM|Network")
    void SetMaxPingThreshold(int32 MaxPing);

    UFUNCTION(BlueprintCallable, Category = "UPM|Network")
    void SetNetworkSmoothing(float Smoothing);

    UFUNCTION(BlueprintCallable, Category = "UPM|Network")
    void SetBandwidthLimit(int32 LimitKBps);

    UFUNCTION(BlueprintCallable, Category = "UPM|Network")
    void SetPreferredRegion(const FString& Region);

    UFUNCTION(BlueprintCallable, Category = "UPM|Network")
    void SetCrossplayEnabled(bool bEnabled);

    // ==================== Debug Settings ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Debug")
    void SetDebugSettings(const FUPMDebugSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "UPM|Debug")
    void SetPerformanceOverlayVisible(bool bVisible);

    UFUNCTION(BlueprintCallable, Category = "UPM|Debug")
    void SetNetworkStatsVisible(bool bVisible);

    UFUNCTION(BlueprintCallable, Category = "UPM|Debug")
    void SetDeveloperMode(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Debug")
    void SetCrashReportingEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UPM|Debug")
    void SetBenchmarkMode(bool bEnabled);

    // ==================== Convenient Wrapper Functions ====================
    // Simple, commonly-used functions for quick setup

    /**
     * Set overall graphics quality (0-4: Low, Medium, High, Ultra, Epic)
     * Sets all graphics settings to the same quality level
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Quick")
    void SetOverallGraphicsQuality(int32 Quality);

    /**
     * Set maximum frame rate (0 = unlimited)
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Quick")
    void SetMaxFrameRate(float MaxFPS);

    /**
     * Enable/disable ray tracing
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Quick")
    void SetRayTracing(bool bEnable);

    /**
     * Set resolution by width and height
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Quick")
    void SetResolutionSimple(int32 Width, int32 Height);

    /**
     * Toggle fullscreen mode
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Quick")
    void SetFullscreen(bool bFullscreen);

    /**
     * Set master audio volume (0.0 - 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Quick")
    void SetVolume(float Volume);

    /**
     * Apply quality preset: 0=Low, 1=Medium, 2=High, 3=Ultra, 4=Epic
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Quick")
    void ApplyQualityPreset(int32 PresetLevel);

    /**
     * Enable/disable all post-process effects at once
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Quick")
    void SetAllPostProcessEffects(bool bEnable);

    /**
     * Quick performance mode - disables expensive features
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Quick")
    void EnablePerformanceMode(bool bEnable);

    /**
     * Quick quality mode - enables all visual features
     */
    UFUNCTION(BlueprintCallable, Category = "UPM|Quick")
    void EnableQualityMode(bool bEnable);

    // ==================== Persistence ====================

    UFUNCTION(BlueprintCallable, Category = "UPM|Persistence")
    bool SaveSettings();

    UFUNCTION(BlueprintCallable, Category = "UPM|Persistence")
    bool LoadSettings();

    UFUNCTION(BlueprintCallable, Category = "UPM|Persistence")
    void Initialize();

protected:
    UPROPERTY(BlueprintReadOnly, Category = "UPM|Performance")
    FUPMPerformanceMetrics PerformanceMetrics;

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
    void ApplyAccessibilitySettings();
    void ApplyNetworkSettings();
    void ApplyDebugSettings();

    // Persistence helpers
    FString GetSettingsFilePath() const;
    TSharedPtr<FJsonObject> SettingsToJson() const;
    bool JsonToSettings(TSharedPtr<FJsonObject> JsonObject);

    // Singleton instance
    static UUPMSettingsManager* Instance;
};
