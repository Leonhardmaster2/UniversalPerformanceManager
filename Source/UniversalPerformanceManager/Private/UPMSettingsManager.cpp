// Copyright Universal Performance Manager. All Rights Reserved.

#include "UPMSettingsManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HAL/PlatformMemory.h"
#include "HAL/PlatformProcess.h"
#include "RHI.h"
#include "RHIStats.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

// Initialize static instance
UUPMSettingsManager* UUPMSettingsManager::Instance = nullptr;

UUPMSettingsManager::UUPMSettingsManager()
    : FPSHistoryTimeAccumulator(0.0f)
{
    FPSHistory.Reserve(120); // Reserve space for 120 frames (2 seconds at 60 FPS)
}

// ==================== Singleton Access ====================

UUPMSettingsManager* UUPMSettingsManager::GetInstance(const UObject* WorldContextObject)
{
    if (!Instance)
    {
        if (!WorldContextObject)
        {
            return nullptr;
        }

        UWorld* World = WorldContextObject->GetWorld();
        if (!World)
        {
            return nullptr;
        }

        Instance = NewObject<UUPMSettingsManager>();
        if (Instance)
        {
            Instance->AddToRoot(); // Prevent garbage collection
            Instance->Initialize();
        }
    }
    return Instance;
}

// ==================== Initialization ====================

void UUPMSettingsManager::Initialize()
{
    // Load settings from disk
    LoadSettings();

    // Apply loaded settings
    ApplyAllSettings();
}

// ==================== Performance Monitoring ====================

void UUPMSettingsManager::UpdatePerformanceMetrics(float DeltaTime)
{
    if (DeltaTime <= 0.0f)
    {
        return;
    }

    // Calculate current FPS
    PerformanceMetrics.FPS_Current = 1.0f / DeltaTime;

    // Update FPS history
    FPSHistory.Add(PerformanceMetrics.FPS_Current);
    FPSHistoryTimeAccumulator += DeltaTime;

    // Keep only last 2 seconds of data
    if (FPSHistoryTimeAccumulator > 2.0f)
    {
        if (FPSHistory.Num() > 60)
        {
            FPSHistory.RemoveAt(0, FPSHistory.Num() - 120);
        }
        FPSHistoryTimeAccumulator = 2.0f;
    }

    // Calculate FPS statistics
    if (FPSHistory.Num() > 0)
    {
        float Sum = 0.0f;
        float Min = FPSHistory[0];
        float Max = FPSHistory[0];

        for (float FPS : FPSHistory)
        {
            Sum += FPS;
            Min = FMath::Min(Min, FPS);
            Max = FMath::Max(Max, FPS);
        }

        PerformanceMetrics.FPS_Average = Sum / FPSHistory.Num();
        PerformanceMetrics.FPS_Min = Min;
        PerformanceMetrics.FPS_Max = Max;
    }

    // CPU and GPU frame times
    PerformanceMetrics.CPUFrameTime = DeltaTime * 1000.0f; // Convert to milliseconds
    PerformanceMetrics.GPUFrameTime = DeltaTime * 1000.0f * 0.8f; // Rough estimate

    // Memory usage
    FPlatformMemoryStats MemoryStats = FPlatformMemory::GetStats();
    PerformanceMetrics.RAMUsageMB = static_cast<float>(MemoryStats.UsedPhysical) / (1024.0f * 1024.0f);

    // VRAM usage (if available)
    if (GDynamicRHI)
    {
        uint64 UsedVRAM = 0;
        RHIGetResourceMemoryUsage(UsedVRAM);

        if (UsedVRAM > 0)
        {
            PerformanceMetrics.VRAMUsageMB = static_cast<float>(UsedVRAM) / (1024.0f * 1024.0f);
        }
    }

    // Thread load (normalized 0-1)
    PerformanceMetrics.GameThreadLoad = FMath::Clamp(DeltaTime / 0.0166f, 0.0f, 1.0f); // 60 FPS baseline
    PerformanceMetrics.RenderThreadLoad = PerformanceMetrics.GameThreadLoad * 0.9f;
    PerformanceMetrics.RHIThreadLoad = PerformanceMetrics.GameThreadLoad * 0.7f;
}

void UUPMSettingsManager::ResetPerformanceStats()
{
    FPSHistory.Empty();
    FPSHistoryTimeAccumulator = 0.0f;
    PerformanceMetrics.FPS_Min = 999.0f;
    PerformanceMetrics.FPS_Max = 0.0f;
    PerformanceMetrics.FPS_Average = 0.0f;
}

// ==================== Settings Application ====================

void UUPMSettingsManager::ApplyAllSettings()
{
    ApplyGraphicsSettings();
    ApplyRenderingSettings();
    ApplyPerformanceSettings();
    ApplyDisplaySettings();
    ApplyAudioSettings();
    ApplyGameplaySettings();
    ApplyAccessibilitySettings();
    ApplyNetworkSettings();
    ApplyDebugSettings();
}

// ==================== Graphics Settings ====================

void UUPMSettingsManager::SetGraphicsSettings(const FUPMGraphicsSettings& Settings)
{
    CurrentSettings.Graphics = Settings;
    ApplyGraphicsSettings();
}

void UUPMSettingsManager::SetAntiAliasingQuality(int32 Quality)
{
    CurrentSettings.Graphics.AntiAliasingQuality = FMath::Clamp(Quality, 0, 4);
    ApplyGraphicsSettings();
}

void UUPMSettingsManager::SetShadowQuality(int32 Quality)
{
    CurrentSettings.Graphics.ShadowQuality = FMath::Clamp(Quality, 0, 4);
    ApplyGraphicsSettings();
}

void UUPMSettingsManager::SetViewDistanceQuality(int32 Quality)
{
    CurrentSettings.Graphics.ViewDistanceQuality = FMath::Clamp(Quality, 0, 4);
    ApplyGraphicsSettings();
}

void UUPMSettingsManager::SetPostProcessQuality(int32 Quality)
{
    CurrentSettings.Graphics.PostProcessQuality = FMath::Clamp(Quality, 0, 4);
    ApplyGraphicsSettings();
}

void UUPMSettingsManager::SetTextureQuality(int32 Quality)
{
    CurrentSettings.Graphics.TextureQuality = FMath::Clamp(Quality, 0, 4);
    ApplyGraphicsSettings();
}

void UUPMSettingsManager::ApplyGraphicsSettings()
{
    UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings();
    if (!GameSettings)
    {
        return;
    }

    // Apply scalability settings
    GameSettings->SetAntiAliasingQuality(CurrentSettings.Graphics.AntiAliasingQuality);
    GameSettings->SetShadowQuality(CurrentSettings.Graphics.ShadowQuality);
    GameSettings->SetViewDistanceQuality(CurrentSettings.Graphics.ViewDistanceQuality);
    GameSettings->SetPostProcessingQuality(CurrentSettings.Graphics.PostProcessQuality);
    GameSettings->SetTextureQuality(CurrentSettings.Graphics.TextureQuality);
    GameSettings->SetVisualEffectQuality(CurrentSettings.Graphics.EffectsQuality);
    GameSettings->SetFoliageQuality(CurrentSettings.Graphics.FoliageQuality);
    GameSettings->SetShadingQuality(CurrentSettings.Graphics.ShadingQuality);

    // Apply immediately without restart
    GameSettings->ApplySettings(false);
}

// ==================== Rendering Settings (EXPANDED) ====================

void UUPMSettingsManager::SetRenderingSettings(const FUPMRenderingSettings& Settings)
{
    CurrentSettings.Rendering = Settings;
    ApplyRenderingSettings();
}

#define UPM_SET_RENDERING_BOOL(VarName, MemberName) \
void UUPMSettingsManager::Set##VarName##Enabled(bool bEnabled) \
{ \
    CurrentSettings.Rendering.bEnable##VarName = bEnabled; \
    ApplyRenderingSettings(); \
}

UPM_SET_RENDERING_BOOL(Lumen, Lumen)
UPM_SET_RENDERING_BOOL(RayTracing, RayTracing)
UPM_SET_RENDERING_BOOL(SSAO, SSAO)
UPM_SET_RENDERING_BOOL(SSR, SSR)
UPM_SET_RENDERING_BOOL(DepthOfField, DepthOfField)
UPM_SET_RENDERING_BOOL(LensFlares, LensFlares)
UPM_SET_RENDERING_BOOL(ChromaticAberration, ChromaticAberration)
UPM_SET_RENDERING_BOOL(FilmGrain, FilmGrain)
UPM_SET_RENDERING_BOOL(Vignette, Vignette)
UPM_SET_RENDERING_BOOL(VolumetricFog, VolumetricFog)
UPM_SET_RENDERING_BOOL(TAA, TAA)
UPM_SET_RENDERING_BOOL(SSGI, SSGI)
UPM_SET_RENDERING_BOOL(ContactShadows, ContactShadows)

#undef UPM_SET_RENDERING_BOOL

void UUPMSettingsManager::SetAnisotropicFiltering(int32 Level)
{
    CurrentSettings.Rendering.AnisotropicFiltering = FMath::Clamp(Level, 0, 4);
    ApplyRenderingSettings();
}

void UUPMSettingsManager::SetUpscalingMode(EUPMUpscalingMode Mode)
{
    CurrentSettings.Rendering.UpscalingMode = Mode;
    ApplyRenderingSettings();
}

void UUPMSettingsManager::SetGlobalIlluminationQuality(int32 Quality)
{
    CurrentSettings.Rendering.GlobalIlluminationQuality = FMath::Clamp(Quality, 0, 4);
    ApplyRenderingSettings();
}

void UUPMSettingsManager::SetReflectionQuality(int32 Quality)
{
    CurrentSettings.Rendering.ReflectionQuality = FMath::Clamp(Quality, 0, 4);
    ApplyRenderingSettings();
}

void UUPMSettingsManager::ApplyRenderingSettings()
{
    if (!GEngine)
    {
        return;
    }

    // Helper macro for setting console variables
    #define SET_CVAR_INT(Name, Value) \
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT(Name))) \
            CVar->Set(Value);

    #define SET_CVAR_FLOAT(Name, Value) \
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT(Name))) \
            CVar->Set(Value);

    // Original settings
    SET_CVAR_INT("r.Lumen.DiffuseIndirect.Allow", CurrentSettings.Rendering.bEnableLumen ? 1 : 0);
    SET_CVAR_INT("r.RayTracing", CurrentSettings.Rendering.bEnableRayTracing ? 1 : 0);
    SET_CVAR_INT("r.AmbientOcclusionLevels", CurrentSettings.Rendering.bEnableSSAO ? 3 : 0);
    SET_CVAR_INT("r.SSR.Quality", CurrentSettings.Rendering.bEnableSSR ? 3 : 0);
    SET_CVAR_INT("r.MotionBlurQuality", CurrentSettings.Rendering.bEnableMotionBlur ? 4 : 0);
    SET_CVAR_INT("r.BloomQuality", CurrentSettings.Rendering.bEnableBloom ? 5 : 0);

    // NEW: Post-process effects
    SET_CVAR_INT("r.DepthOfFieldQuality", CurrentSettings.Rendering.bEnableDepthOfField ? 2 : 0);
    SET_CVAR_INT("r.LensFlareQuality", CurrentSettings.Rendering.bEnableLensFlares ? 2 : 0);
    SET_CVAR_FLOAT("r.SceneColorFringe.Max", CurrentSettings.Rendering.bEnableChromaticAberration ? 5.0f : 0.0f);
    SET_CVAR_FLOAT("r.Tonemapper.GrainQuantization", CurrentSettings.Rendering.bEnableFilmGrain ? 1.0f : 0.0f);
    SET_CVAR_FLOAT("r.Tonemapper.Vignette", CurrentSettings.Rendering.bEnableVignette ? 0.4f : 0.0f);

    // NEW: Quality settings
    SET_CVAR_INT("r.VolumetricFog", CurrentSettings.Rendering.bEnableVolumetricFog ? 1 : 0);

    // Anisotropic filtering (convert level to power of 2)
    int32 AnisotropyValue = (CurrentSettings.Rendering.AnisotropicFiltering > 0) ?
        (1 << CurrentSettings.Rendering.AnisotropicFiltering) : 0;
    SET_CVAR_INT("r.MaxAnisotropy", AnisotropyValue);

    // TAA
    SET_CVAR_INT("r.TemporalAA.Quality", CurrentSettings.Rendering.bEnableTAA ? 2 : 0);

    // Upscaling (these would depend on what plugins are available)
    switch (CurrentSettings.Rendering.UpscalingMode)
    {
    case EUPMUpscalingMode::TSR:
        SET_CVAR_INT("r.TemporalSuperResolution", 1);
        break;
    case EUPMUpscalingMode::DLSS:
        SET_CVAR_INT("r.NGX.DLSS.Enable", 1);
        break;
    case EUPMUpscalingMode::FSR:
        SET_CVAR_INT("r.FidelityFX.FSR.Enabled", 1);
        break;
    default:
        break;
    }

    // GI and Reflections quality
    SET_CVAR_INT("r.Lumen.Reflections.ScreenTraces", CurrentSettings.Rendering.GlobalIlluminationQuality);
    SET_CVAR_INT("r.ReflectionEnvironment", CurrentSettings.Rendering.ReflectionQuality > 0 ? 1 : 0);

    // SSGI
    SET_CVAR_INT("r.SSGI.Enable", CurrentSettings.Rendering.bEnableSSGI ? 1 : 0);

    // Contact shadows
    SET_CVAR_INT("r.ContactShadows", CurrentSettings.Rendering.bEnableContactShadows ? 1 : 0);

    #undef SET_CVAR_INT
    #undef SET_CVAR_FLOAT
}

// ==================== Performance Settings (EXPANDED) ====================

void UUPMSettingsManager::SetPerformanceSettings(const FUPMPerformanceSettings& Settings)
{
    CurrentSettings.Performance = Settings;
    ApplyPerformanceSettings();
}

void UUPMSettingsManager::SetVSyncEnabled(bool bEnabled)
{
    CurrentSettings.Performance.bEnableVSync = bEnabled;
    ApplyPerformanceSettings();
}

void UUPMSettingsManager::SetFrameRateLimit(float Limit)
{
    CurrentSettings.Performance.FrameRateLimit = FMath::Max(0.0f, Limit);
    ApplyPerformanceSettings();
}

void UUPMSettingsManager::SetDynamicResolutionEnabled(bool bEnabled)
{
    CurrentSettings.Performance.bEnableDynamicResolution = bEnabled;
    ApplyPerformanceSettings();
}

void UUPMSettingsManager::SetMinFrameRateForDynamicRes(float MinFPS)
{
    CurrentSettings.Performance.MinFrameRateForDynamicRes = FMath::Clamp(MinFPS, 15.0f, 60.0f);
    ApplyPerformanceSettings();
}

void UUPMSettingsManager::SetTripleBufferingEnabled(bool bEnabled)
{
    CurrentSettings.Performance.bEnableTripleBuffering = bEnabled;
    ApplyPerformanceSettings();
}

void UUPMSettingsManager::SetAsyncComputeEnabled(bool bEnabled)
{
    CurrentSettings.Performance.bEnableAsyncCompute = bEnabled;
    ApplyPerformanceSettings();
}

void UUPMSettingsManager::SetLODDistanceMultiplier(float Multiplier)
{
    CurrentSettings.Performance.LODDistanceMultiplier = FMath::Clamp(Multiplier, 0.25f, 4.0f);
    ApplyPerformanceSettings();
}

void UUPMSettingsManager::SetProcessPriority(int32 Priority)
{
    CurrentSettings.Performance.ProcessPriority = FMath::Clamp(Priority, 0, 2);
    ApplyPerformanceSettings();
}

void UUPMSettingsManager::ApplyPerformanceSettings()
{
    UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings();
    if (GameSettings)
    {
        // Original settings
        GameSettings->SetVSyncEnabled(CurrentSettings.Performance.bEnableVSync);

        if (CurrentSettings.Performance.FrameRateLimit > 0.0f)
        {
            GameSettings->SetFrameRateLimit(CurrentSettings.Performance.FrameRateLimit);
        }
        else
        {
            GameSettings->SetFrameRateLimit(0.0f);
        }

        GameSettings->ApplySettings(false);
    }

    // Console variables
    #define SET_CVAR_INT(Name, Value) \
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT(Name))) \
            CVar->Set(Value);

    #define SET_CVAR_FLOAT(Name, Value) \
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT(Name))) \
            CVar->Set(Value);

    SET_CVAR_INT("r.VSync", CurrentSettings.Performance.bEnableVSync ? 1 : 0);
    SET_CVAR_FLOAT("t.MaxFPS", CurrentSettings.Performance.FrameRateLimit);

    // NEW: Dynamic resolution
    SET_CVAR_INT("r.DynamicRes.OperationMode", CurrentSettings.Performance.bEnableDynamicResolution ? 2 : 0);
    SET_CVAR_FLOAT("r.DynamicRes.MinResolutionChangesPerSecond",
        1000.0f / (CurrentSettings.Performance.MinFrameRateForDynamicRes + 0.01f));

    // NEW: Triple buffering (RHI-dependent)
    SET_CVAR_INT("r.MaxFrameLatency", CurrentSettings.Performance.bEnableTripleBuffering ? 3 : 2);

    // NEW: Async compute
    SET_CVAR_INT("r.AsyncCompute", CurrentSettings.Performance.bEnableAsyncCompute ? 1 : 0);

    // NEW: LOD distance multiplier
    SET_CVAR_FLOAT("r.ViewDistanceScale", CurrentSettings.Performance.LODDistanceMultiplier);

    // NEW: Process priority
    if (CurrentSettings.Performance.ProcessPriority == 1)
    {
        FPlatformProcess::SetThreadPriority(FPlatformProcess::GetCurrentThread(), TPri_AboveNormal);
    }
    else if (CurrentSettings.Performance.ProcessPriority == 2)
    {
        FPlatformProcess::SetThreadPriority(FPlatformProcess::GetCurrentThread(), TPri_Highest);
    }

    #undef SET_CVAR_INT
    #undef SET_CVAR_FLOAT
}

// ==================== Display Settings (EXPANDED) ====================

void UUPMSettingsManager::SetDisplaySettings(const FUPMDisplaySettings& Settings)
{
    CurrentSettings.Display = Settings;
    ApplyDisplaySettings();
}

void UUPMSettingsManager::SetResolution(FIntPoint Resolution)
{
    CurrentSettings.Display.Resolution = Resolution;
    ApplyDisplaySettings();
}

void UUPMSettingsManager::SetWindowMode(TEnumAsByte<EWindowMode::Type> Mode)
{
    CurrentSettings.Display.WindowMode = Mode;
    ApplyDisplaySettings();
}

void UUPMSettingsManager::SetBrightness(float Brightness)
{
    CurrentSettings.Display.Brightness = FMath::Clamp(Brightness, 0.0f, 2.0f);
    ApplyDisplaySettings();
}

void UUPMSettingsManager::SetContrast(float Contrast)
{
    CurrentSettings.Display.Contrast = FMath::Clamp(Contrast, 0.0f, 2.0f);
    ApplyDisplaySettings();
}

void UUPMSettingsManager::SetHDREnabled(bool bEnabled)
{
    CurrentSettings.Display.bEnableHDR = bEnabled;
    ApplyDisplaySettings();
}

void UUPMSettingsManager::SetHDRMaxNits(float MaxNits)
{
    CurrentSettings.Display.HDRMaxNits = FMath::Clamp(MaxNits, 1000.0f, 10000.0f);
    ApplyDisplaySettings();
}

void UUPMSettingsManager::SetMonitorIndex(int32 Index)
{
    CurrentSettings.Display.MonitorIndex = FMath::Max(0, Index);
    ApplyDisplaySettings();
}

void UUPMSettingsManager::SetBorderlessWindow(bool bBorderless)
{
    CurrentSettings.Display.bBorderlessWindow = bBorderless;
    ApplyDisplaySettings();
}

void UUPMSettingsManager::SetScreenPercentage(float Percentage)
{
    CurrentSettings.Display.ScreenPercentage = FMath::Clamp(Percentage, 50.0f, 200.0f);
    ApplyDisplaySettings();
}

void UUPMSettingsManager::SetMenuFieldOfView(float FOV)
{
    CurrentSettings.Display.MenuFieldOfView = FMath::Clamp(FOV, 60.0f, 120.0f);
    ApplyDisplaySettings();
}

void UUPMSettingsManager::SetAspectRatioOverride(float AspectRatio)
{
    CurrentSettings.Display.AspectRatioOverride = AspectRatio;
    ApplyDisplaySettings();
}

void UUPMSettingsManager::SetSafeZoneScale(float Scale)
{
    CurrentSettings.Display.SafeZoneScale = FMath::Clamp(Scale, 0.8f, 1.0f);
    ApplyDisplaySettings();
}

void UUPMSettingsManager::ApplyDisplaySettings()
{
    UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings();
    if (GameSettings)
    {
        // Original settings
        GameSettings->SetScreenResolution(CurrentSettings.Display.Resolution);
        GameSettings->SetFullscreenMode(CurrentSettings.Display.WindowMode);
        GameSettings->ApplySettings(false);
    }

    // Console variables for new settings
    #define SET_CVAR_FLOAT(Name, Value) \
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT(Name))) \
            CVar->Set(Value);

    #define SET_CVAR_INT(Name, Value) \
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT(Name))) \
            CVar->Set(Value);

    // NEW: Brightness and contrast (post-process)
    SET_CVAR_FLOAT("r.Tonemapper.Sharpen", CurrentSettings.Display.Brightness - 1.0f);

    // NEW: HDR
    SET_CVAR_INT("r.HDR.EnableHDROutput", CurrentSettings.Display.bEnableHDR ? 1 : 0);
    SET_CVAR_FLOAT("r.HDR.Display.OutputDevice", CurrentSettings.Display.HDRMaxNits);

    // NEW: Screen percentage
    SET_CVAR_FLOAT("r.ScreenPercentage", CurrentSettings.Display.ScreenPercentage);

    #undef SET_CVAR_FLOAT
    #undef SET_CVAR_INT
}

// ==================== Audio Settings (EXPANDED) ====================

void UUPMSettingsManager::SetAudioSettings(const FUPMAudioSettings& Settings)
{
    CurrentSettings.Audio = Settings;
    ApplyAudioSettings();
}

#define UPM_SET_AUDIO_VOLUME(VarName) \
void UUPMSettingsManager::Set##VarName##Volume(float Volume) \
{ \
    CurrentSettings.Audio.VarName##Volume = FMath::Clamp(Volume, 0.0f, 1.0f); \
    ApplyAudioSettings(); \
}

UPM_SET_AUDIO_VOLUME(Master)
UPM_SET_AUDIO_VOLUME(SFX)
UPM_SET_AUDIO_VOLUME(Music)
UPM_SET_AUDIO_VOLUME(VoiceDialog)
UPM_SET_AUDIO_VOLUME(Ambient)
UPM_SET_AUDIO_VOLUME(UISound)
UPM_SET_AUDIO_VOLUME(VoiceChat)

#undef UPM_SET_AUDIO_VOLUME

void UUPMSettingsManager::SetAudioQuality(int32 Quality)
{
    CurrentSettings.Audio.AudioQuality = FMath::Clamp(Quality, 0, 3);
    ApplyAudioSettings();
}

void UUPMSettingsManager::SetSurroundSoundMode(int32 Mode)
{
    CurrentSettings.Audio.SurroundSoundMode = FMath::Clamp(Mode, 0, 2);
    ApplyAudioSettings();
}

void UUPMSettingsManager::SetSpatialAudioEnabled(bool bEnabled)
{
    CurrentSettings.Audio.bEnableSpatialAudio = bEnabled;
    ApplyAudioSettings();
}

void UUPMSettingsManager::SetDynamicRange(float Range)
{
    CurrentSettings.Audio.DynamicRange = FMath::Clamp(Range, 0.0f, 1.0f);
    ApplyAudioSettings();
}

void UUPMSettingsManager::SetSubtitleTextSize(float Size)
{
    CurrentSettings.Audio.SubtitleTextSize = FMath::Clamp(Size, 0.5f, 2.0f);
    ApplyAudioSettings();
}

void UUPMSettingsManager::SetSubtitleBackgroundOpacity(float Opacity)
{
    CurrentSettings.Audio.SubtitleBackgroundOpacity = FMath::Clamp(Opacity, 0.0f, 1.0f);
    ApplyAudioSettings();
}

void UUPMSettingsManager::ApplyAudioSettings()
{
    // Audio application depends on your audio system
    // This is a placeholder for common audio system integration

    #define SET_CVAR_FLOAT(Name, Value) \
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT(Name))) \
            CVar->Set(Value);

    // Use console variables if your project uses them
    SET_CVAR_FLOAT("au.MasterVolume", CurrentSettings.Audio.MasterVolume);

    #undef SET_CVAR_FLOAT

    // For Sound Classes or Audio Mixer:
    // You would load your sound classes and set volumes here
    // Example:
    // if (USoundClass* MasterSoundClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/Audio/MasterSoundClass")))
    // {
    //     UGameplayStatics::SetSoundClassVolume(this, MasterSoundClass, CurrentSettings.Audio.MasterVolume);
    // }
}

// ==================== Gameplay Settings (EXPANDED) ====================

void UUPMSettingsManager::SetGameplaySettings(const FUPMGameplaySettings& Settings)
{
    CurrentSettings.Gameplay = Settings;
    ApplyGameplaySettings();
}

void UUPMSettingsManager::SetFOV(float FOV)
{
    CurrentSettings.Gameplay.FOV = FMath::Clamp(FOV, 60.0f, 120.0f);
    ApplyGameplaySettings();
}

void UUPMSettingsManager::SetMouseSensitivity(float Sensitivity)
{
    CurrentSettings.Gameplay.MouseSensitivity = FMath::Clamp(Sensitivity, 0.1f, 5.0f);
    ApplyGameplaySettings();
}

void UUPMSettingsManager::SetControllerSensitivity(float Sensitivity)
{
    CurrentSettings.Gameplay.ControllerSensitivity = FMath::Clamp(Sensitivity, 0.1f, 5.0f);
    ApplyGameplaySettings();
}

void UUPMSettingsManager::SetControllerDeadZone(float DeadZone)
{
    CurrentSettings.Gameplay.ControllerDeadZone = FMath::Clamp(DeadZone, 0.0f, 0.5f);
    ApplyGameplaySettings();
}

void UUPMSettingsManager::SetAimAssistStrength(float Strength)
{
    CurrentSettings.Gameplay.AimAssistStrength = FMath::Clamp(Strength, 0.0f, 1.0f);
    ApplyGameplaySettings();
}

void UUPMSettingsManager::SetCameraShakeIntensity(float Intensity)
{
    CurrentSettings.Gameplay.CameraShakeIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
    ApplyGameplaySettings();
}

void UUPMSettingsManager::SetHeadBobIntensity(float Intensity)
{
    CurrentSettings.Gameplay.HeadBobIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
    ApplyGameplaySettings();
}

void UUPMSettingsManager::SetVibrationEnabled(bool bEnabled)
{
    CurrentSettings.Gameplay.bEnableVibration = bEnabled;
    ApplyGameplaySettings();
}

void UUPMSettingsManager::SetCrouchToggle(bool bToggle)
{
    CurrentSettings.Gameplay.bCrouchToggle = bToggle;
    ApplyGameplaySettings();
}

void UUPMSettingsManager::SetSprintToggle(bool bToggle)
{
    CurrentSettings.Gameplay.bSprintToggle = bToggle;
    ApplyGameplaySettings();
}

void UUPMSettingsManager::SetAutoRunEnabled(bool bEnabled)
{
    CurrentSettings.Gameplay.bEnableAutoRun = bEnabled;
    ApplyGameplaySettings();
}

void UUPMSettingsManager::SetCameraSmoothing(float Smoothing)
{
    CurrentSettings.Gameplay.CameraSmoothing = FMath::Clamp(Smoothing, 0.0f, 1.0f);
    ApplyGameplaySettings();
}

void UUPMSettingsManager::ApplyGameplaySettings()
{
    // Gameplay settings are typically applied through game-specific systems
    // You would notify your game systems about changes here

    // This is where you would broadcast delegates or update game-specific components
    // For example:
    // - FOV: Applied to camera component
    // - Mouse Sensitivity: Applied to player controller input
    // - Controller settings: Applied to input system
}

// ==================== Accessibility Settings (NEW) ====================

void UUPMSettingsManager::SetAccessibilitySettings(const FUPMAccessibilitySettings& Settings)
{
    CurrentSettings.Accessibility = Settings;
    ApplyAccessibilitySettings();
}

void UUPMSettingsManager::SetColorblindMode(EUPMColorblindMode Mode)
{
    CurrentSettings.Accessibility.ColorblindMode = Mode;
    ApplyAccessibilitySettings();
}

void UUPMSettingsManager::SetUIScale(float Scale)
{
    CurrentSettings.Accessibility.UIScale = FMath::Clamp(Scale, 0.5f, 2.0f);
    ApplyAccessibilitySettings();
}

void UUPMSettingsManager::SetTextSize(float Size)
{
    CurrentSettings.Accessibility.TextSize = FMath::Clamp(Size, 0.5f, 2.0f);
    ApplyAccessibilitySettings();
}

void UUPMSettingsManager::SetHighContrastMode(bool bEnabled)
{
    CurrentSettings.Accessibility.bHighContrastMode = bEnabled;
    ApplyAccessibilitySettings();
}

void UUPMSettingsManager::SetScreenReaderEnabled(bool bEnabled)
{
    CurrentSettings.Accessibility.bEnableScreenReader = bEnabled;
    ApplyAccessibilitySettings();
}

void UUPMSettingsManager::SetReducedMotion(bool bEnabled)
{
    CurrentSettings.Accessibility.bReducedMotion = bEnabled;
    ApplyAccessibilitySettings();
}

void UUPMSettingsManager::SetPhotosensitivityMode(bool bEnabled)
{
    CurrentSettings.Accessibility.bPhotosensitivityMode = bEnabled;
    ApplyAccessibilitySettings();
}

void UUPMSettingsManager::ApplyAccessibilitySettings()
{
    #define SET_CVAR_INT(Name, Value) \
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT(Name))) \
            CVar->Set(Value);

    #define SET_CVAR_FLOAT(Name, Value) \
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT(Name))) \
            CVar->Set(Value);

    // Colorblind mode (would need custom post-process material in real implementation)
    int32 ColorblindValue = static_cast<int32>(CurrentSettings.Accessibility.ColorblindMode);
    SET_CVAR_INT("r.ColorBlind.Mode", ColorblindValue);

    // Photosensitivity mode - reduce flashing effects
    if (CurrentSettings.Accessibility.bPhotosensitivityMode)
    {
        SET_CVAR_INT("r.BloomQuality", 0);
        SET_CVAR_FLOAT("r.MotionBlurQuality", 0);
        SET_CVAR_INT("r.LensFlareQuality", 0);
    }

    // Reduced motion
    if (CurrentSettings.Accessibility.bReducedMotion)
    {
        SET_CVAR_FLOAT("r.MotionBlurQuality", 0);
    }

    #undef SET_CVAR_INT
    #undef SET_CVAR_FLOAT

    // UI Scale and Text Size would be applied to your UI system
    // You would broadcast these changes to your widget manager
}

// ==================== Network Settings (NEW) ====================

void UUPMSettingsManager::SetNetworkSettings(const FUPMNetworkSettings& Settings)
{
    CurrentSettings.Network = Settings;
    ApplyNetworkSettings();
}

void UUPMSettingsManager::SetMaxPingThreshold(int32 MaxPing)
{
    CurrentSettings.Network.MaxPingThreshold = FMath::Max(0, MaxPing);
    ApplyNetworkSettings();
}

void UUPMSettingsManager::SetNetworkSmoothing(float Smoothing)
{
    CurrentSettings.Network.NetworkSmoothing = FMath::Clamp(Smoothing, 0.0f, 1.0f);
    ApplyNetworkSettings();
}

void UUPMSettingsManager::SetBandwidthLimit(int32 LimitKBps)
{
    CurrentSettings.Network.BandwidthLimitKBps = FMath::Max(0, LimitKBps);
    ApplyNetworkSettings();
}

void UUPMSettingsManager::SetPreferredRegion(const FString& Region)
{
    CurrentSettings.Network.PreferredRegion = Region;
    ApplyNetworkSettings();
}

void UUPMSettingsManager::SetCrossplayEnabled(bool bEnabled)
{
    CurrentSettings.Network.bEnableCrossplay = bEnabled;
    ApplyNetworkSettings();
}

void UUPMSettingsManager::ApplyNetworkSettings()
{
    // Network settings would be applied through your multiplayer/networking system
    // These are typically game-specific and would require integration with your network manager

    #define SET_CVAR_FLOAT(Name, Value) \
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT(Name))) \
            CVar->Set(Value);

    // Network smoothing (client-side prediction)
    SET_CVAR_FLOAT("p.NetClientInterpolation", CurrentSettings.Network.NetworkSmoothing);

    #undef SET_CVAR_FLOAT
}

// ==================== Debug Settings (NEW) ====================

void UUPMSettingsManager::SetDebugSettings(const FUPMDebugSettings& Settings)
{
    CurrentSettings.Debug = Settings;
    ApplyDebugSettings();
}

void UUPMSettingsManager::SetPerformanceOverlayVisible(bool bVisible)
{
    CurrentSettings.Debug.bShowPerformanceOverlay = bVisible;
    ApplyDebugSettings();
}

void UUPMSettingsManager::SetNetworkStatsVisible(bool bVisible)
{
    CurrentSettings.Debug.bShowNetworkStats = bVisible;
    ApplyDebugSettings();
}

void UUPMSettingsManager::SetDeveloperMode(bool bEnabled)
{
    CurrentSettings.Debug.bDeveloperMode = bEnabled;
    ApplyDebugSettings();
}

void UUPMSettingsManager::SetCrashReportingEnabled(bool bEnabled)
{
    CurrentSettings.Debug.bEnableCrashReporting = bEnabled;
    ApplyDebugSettings();
}

void UUPMSettingsManager::SetBenchmarkMode(bool bEnabled)
{
    CurrentSettings.Debug.bBenchmarkMode = bEnabled;
    ApplyDebugSettings();
}

void UUPMSettingsManager::ApplyDebugSettings()
{
    #define SET_CVAR_INT(Name, Value) \
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT(Name))) \
            CVar->Set(Value);

    // Show stat commands based on debug settings
    SET_CVAR_INT("stat.FPS", CurrentSettings.Debug.bShowPerformanceOverlay ? 1 : 0);
    SET_CVAR_INT("stat.Unit", CurrentSettings.Debug.bShowPerformanceOverlay ? 1 : 0);

    #undef SET_CVAR_INT

    // Benchmark mode would disable various features for consistent testing
    if (CurrentSettings.Debug.bBenchmarkMode)
    {
        // Lock to consistent settings
        if (IConsoleVariable* CVarVSync = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSync")))
        {
            CVarVSync->Set(0);
        }
    }
}

// ==================== Persistence (EXPANDED) ====================

FString UUPMSettingsManager::GetSettingsFilePath() const
{
    return FPaths::ProjectSavedDir() / TEXT("UPM") / TEXT("Settings.json");
}

bool UUPMSettingsManager::SaveSettings()
{
    TSharedPtr<FJsonObject> JsonObject = SettingsToJson();
    if (!JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("UPM: Failed to convert settings to JSON"));
        return false;
    }

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
    {
        UE_LOG(LogTemp, Error, TEXT("UPM: Failed to serialize settings to JSON"));
        return false;
    }

    FString FilePath = GetSettingsFilePath();
    FString DirectoryPath = FPaths::GetPath(FilePath);

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*DirectoryPath))
    {
        if (!PlatformFile.CreateDirectoryTree(*DirectoryPath))
        {
            UE_LOG(LogTemp, Error, TEXT("UPM: Failed to create directory: %s"), *DirectoryPath);
            return false;
        }
    }

    if (!FFileHelper::SaveStringToFile(OutputString, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("UPM: Failed to write settings to file: %s"), *FilePath);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("UPM: Settings saved successfully to: %s"), *FilePath);
    return true;
}

bool UUPMSettingsManager::LoadSettings()
{
    FString FilePath = GetSettingsFilePath();

    if (!FPaths::FileExists(FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("UPM: Settings file not found: %s (using defaults)"), *FilePath);
        return false;
    }

    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("UPM: Failed to read settings file: %s"), *FilePath);
        return false;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("UPM: Failed to parse settings JSON"));
        return false;
    }

    if (!JsonToSettings(JsonObject))
    {
        UE_LOG(LogTemp, Error, TEXT("UPM: Failed to apply settings from JSON"));
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("UPM: Settings loaded successfully from: %s"), *FilePath);
    return true;
}

// Helper macro for JSON serialization
#define JSON_SET_FIELD(Obj, Field, Value) Obj->SetNumberField(TEXT(#Field), Value)
#define JSON_SET_BOOL(Obj, Field, Value) Obj->SetBoolField(TEXT(#Field), Value)
#define JSON_SET_STRING(Obj, Field, Value) Obj->SetStringField(TEXT(#Field), Value)

TSharedPtr<FJsonObject> UUPMSettingsManager::SettingsToJson() const
{
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);

    // Graphics
    TSharedPtr<FJsonObject> GraphicsObject = MakeShareable(new FJsonObject);
    JSON_SET_FIELD(GraphicsObject, AntiAliasingQuality, CurrentSettings.Graphics.AntiAliasingQuality);
    JSON_SET_FIELD(GraphicsObject, ShadowQuality, CurrentSettings.Graphics.ShadowQuality);
    JSON_SET_FIELD(GraphicsObject, ViewDistanceQuality, CurrentSettings.Graphics.ViewDistanceQuality);
    JSON_SET_FIELD(GraphicsObject, PostProcessQuality, CurrentSettings.Graphics.PostProcessQuality);
    JSON_SET_FIELD(GraphicsObject, TextureQuality, CurrentSettings.Graphics.TextureQuality);
    JSON_SET_FIELD(GraphicsObject, EffectsQuality, CurrentSettings.Graphics.EffectsQuality);
    JSON_SET_FIELD(GraphicsObject, FoliageQuality, CurrentSettings.Graphics.FoliageQuality);
    JSON_SET_FIELD(GraphicsObject, ShadingQuality, CurrentSettings.Graphics.ShadingQuality);
    RootObject->SetObjectField("Graphics", GraphicsObject);

    // Rendering (EXPANDED)
    TSharedPtr<FJsonObject> RenderingObject = MakeShareable(new FJsonObject);
    JSON_SET_BOOL(RenderingObject, EnableLumen, CurrentSettings.Rendering.bEnableLumen);
    JSON_SET_BOOL(RenderingObject, EnableRayTracing, CurrentSettings.Rendering.bEnableRayTracing);
    JSON_SET_BOOL(RenderingObject, EnableSSAO, CurrentSettings.Rendering.bEnableSSAO);
    JSON_SET_BOOL(RenderingObject, EnableSSR, CurrentSettings.Rendering.bEnableSSR);
    JSON_SET_BOOL(RenderingObject, EnableMotionBlur, CurrentSettings.Rendering.bEnableMotionBlur);
    JSON_SET_BOOL(RenderingObject, EnableBloom, CurrentSettings.Rendering.bEnableBloom);
    JSON_SET_BOOL(RenderingObject, EnableDepthOfField, CurrentSettings.Rendering.bEnableDepthOfField);
    JSON_SET_BOOL(RenderingObject, EnableLensFlares, CurrentSettings.Rendering.bEnableLensFlares);
    JSON_SET_BOOL(RenderingObject, EnableChromaticAberration, CurrentSettings.Rendering.bEnableChromaticAberration);
    JSON_SET_BOOL(RenderingObject, EnableFilmGrain, CurrentSettings.Rendering.bEnableFilmGrain);
    JSON_SET_BOOL(RenderingObject, EnableVignette, CurrentSettings.Rendering.bEnableVignette);
    JSON_SET_BOOL(RenderingObject, EnableVolumetricFog, CurrentSettings.Rendering.bEnableVolumetricFog);
    JSON_SET_FIELD(RenderingObject, AnisotropicFiltering, CurrentSettings.Rendering.AnisotropicFiltering);
    JSON_SET_BOOL(RenderingObject, EnableTAA, CurrentSettings.Rendering.bEnableTAA);
    RenderingObject->SetNumberField("UpscalingMode", static_cast<int32>(CurrentSettings.Rendering.UpscalingMode));
    JSON_SET_FIELD(RenderingObject, GlobalIlluminationQuality, CurrentSettings.Rendering.GlobalIlluminationQuality);
    JSON_SET_FIELD(RenderingObject, ReflectionQuality, CurrentSettings.Rendering.ReflectionQuality);
    JSON_SET_BOOL(RenderingObject, EnableSSGI, CurrentSettings.Rendering.bEnableSSGI);
    JSON_SET_BOOL(RenderingObject, EnableContactShadows, CurrentSettings.Rendering.bEnableContactShadows);
    RootObject->SetObjectField("Rendering", RenderingObject);

    // Performance (EXPANDED)
    TSharedPtr<FJsonObject> PerformanceObject = MakeShareable(new FJsonObject);
    JSON_SET_BOOL(PerformanceObject, EnableVSync, CurrentSettings.Performance.bEnableVSync);
    JSON_SET_FIELD(PerformanceObject, FrameRateLimit, CurrentSettings.Performance.FrameRateLimit);
    JSON_SET_BOOL(PerformanceObject, EnableDynamicResolution, CurrentSettings.Performance.bEnableDynamicResolution);
    JSON_SET_FIELD(PerformanceObject, MinFrameRateForDynamicRes, CurrentSettings.Performance.MinFrameRateForDynamicRes);
    JSON_SET_BOOL(PerformanceObject, EnableTripleBuffering, CurrentSettings.Performance.bEnableTripleBuffering);
    JSON_SET_BOOL(PerformanceObject, EnableAsyncCompute, CurrentSettings.Performance.bEnableAsyncCompute);
    JSON_SET_FIELD(PerformanceObject, LODDistanceMultiplier, CurrentSettings.Performance.LODDistanceMultiplier);
    JSON_SET_FIELD(PerformanceObject, ProcessPriority, CurrentSettings.Performance.ProcessPriority);
    RootObject->SetObjectField("Performance", PerformanceObject);

    // Display (EXPANDED)
    TSharedPtr<FJsonObject> DisplayObject = MakeShareable(new FJsonObject);
    DisplayObject->SetNumberField("ResolutionX", CurrentSettings.Display.Resolution.X);
    DisplayObject->SetNumberField("ResolutionY", CurrentSettings.Display.Resolution.Y);
    DisplayObject->SetNumberField("WindowMode", static_cast<int32>(CurrentSettings.Display.WindowMode));
    JSON_SET_FIELD(DisplayObject, Brightness, CurrentSettings.Display.Brightness);
    JSON_SET_FIELD(DisplayObject, Contrast, CurrentSettings.Display.Contrast);
    JSON_SET_BOOL(DisplayObject, EnableHDR, CurrentSettings.Display.bEnableHDR);
    JSON_SET_FIELD(DisplayObject, HDRMaxNits, CurrentSettings.Display.HDRMaxNits);
    JSON_SET_FIELD(DisplayObject, MonitorIndex, CurrentSettings.Display.MonitorIndex);
    JSON_SET_BOOL(DisplayObject, BorderlessWindow, CurrentSettings.Display.bBorderlessWindow);
    JSON_SET_FIELD(DisplayObject, ScreenPercentage, CurrentSettings.Display.ScreenPercentage);
    JSON_SET_FIELD(DisplayObject, MenuFieldOfView, CurrentSettings.Display.MenuFieldOfView);
    JSON_SET_FIELD(DisplayObject, AspectRatioOverride, CurrentSettings.Display.AspectRatioOverride);
    JSON_SET_FIELD(DisplayObject, SafeZoneScale, CurrentSettings.Display.SafeZoneScale);
    RootObject->SetObjectField("Display", DisplayObject);

    // Audio (EXPANDED)
    TSharedPtr<FJsonObject> AudioObject = MakeShareable(new FJsonObject);
    JSON_SET_FIELD(AudioObject, MasterVolume, CurrentSettings.Audio.MasterVolume);
    JSON_SET_FIELD(AudioObject, SFXVolume, CurrentSettings.Audio.SFXVolume);
    JSON_SET_FIELD(AudioObject, MusicVolume, CurrentSettings.Audio.MusicVolume);
    JSON_SET_FIELD(AudioObject, VoiceDialogVolume, CurrentSettings.Audio.VoiceDialogVolume);
    JSON_SET_FIELD(AudioObject, AmbientVolume, CurrentSettings.Audio.AmbientVolume);
    JSON_SET_FIELD(AudioObject, UISoundVolume, CurrentSettings.Audio.UISoundVolume);
    JSON_SET_FIELD(AudioObject, VoiceChatVolume, CurrentSettings.Audio.VoiceChatVolume);
    JSON_SET_FIELD(AudioObject, AudioQuality, CurrentSettings.Audio.AudioQuality);
    JSON_SET_FIELD(AudioObject, SurroundSoundMode, CurrentSettings.Audio.SurroundSoundMode);
    JSON_SET_BOOL(AudioObject, EnableSpatialAudio, CurrentSettings.Audio.bEnableSpatialAudio);
    JSON_SET_FIELD(AudioObject, DynamicRange, CurrentSettings.Audio.DynamicRange);
    JSON_SET_FIELD(AudioObject, SubtitleTextSize, CurrentSettings.Audio.SubtitleTextSize);
    JSON_SET_FIELD(AudioObject, SubtitleBackgroundOpacity, CurrentSettings.Audio.SubtitleBackgroundOpacity);
    RootObject->SetObjectField("Audio", AudioObject);

    // Gameplay (EXPANDED)
    TSharedPtr<FJsonObject> GameplayObject = MakeShareable(new FJsonObject);
    JSON_SET_FIELD(GameplayObject, FOV, CurrentSettings.Gameplay.FOV);
    JSON_SET_FIELD(GameplayObject, MouseSensitivity, CurrentSettings.Gameplay.MouseSensitivity);
    JSON_SET_BOOL(GameplayObject, InvertMouseY, CurrentSettings.Gameplay.bInvertMouseY);
    JSON_SET_FIELD(GameplayObject, ControllerSensitivity, CurrentSettings.Gameplay.ControllerSensitivity);
    JSON_SET_FIELD(GameplayObject, ControllerDeadZone, CurrentSettings.Gameplay.ControllerDeadZone);
    JSON_SET_FIELD(GameplayObject, AimAssistStrength, CurrentSettings.Gameplay.AimAssistStrength);
    JSON_SET_FIELD(GameplayObject, CameraShakeIntensity, CurrentSettings.Gameplay.CameraShakeIntensity);
    JSON_SET_FIELD(GameplayObject, HeadBobIntensity, CurrentSettings.Gameplay.HeadBobIntensity);
    JSON_SET_BOOL(GameplayObject, EnableVibration, CurrentSettings.Gameplay.bEnableVibration);
    JSON_SET_BOOL(GameplayObject, CrouchToggle, CurrentSettings.Gameplay.bCrouchToggle);
    JSON_SET_BOOL(GameplayObject, SprintToggle, CurrentSettings.Gameplay.bSprintToggle);
    JSON_SET_BOOL(GameplayObject, EnableAutoRun, CurrentSettings.Gameplay.bEnableAutoRun);
    JSON_SET_FIELD(GameplayObject, CameraSmoothing, CurrentSettings.Gameplay.CameraSmoothing);
    RootObject->SetObjectField("Gameplay", GameplayObject);

    // NEW: Accessibility
    TSharedPtr<FJsonObject> AccessibilityObject = MakeShareable(new FJsonObject);
    AccessibilityObject->SetNumberField("ColorblindMode", static_cast<int32>(CurrentSettings.Accessibility.ColorblindMode));
    JSON_SET_FIELD(AccessibilityObject, UIScale, CurrentSettings.Accessibility.UIScale);
    JSON_SET_FIELD(AccessibilityObject, TextSize, CurrentSettings.Accessibility.TextSize);
    JSON_SET_BOOL(AccessibilityObject, HighContrastMode, CurrentSettings.Accessibility.bHighContrastMode);
    JSON_SET_BOOL(AccessibilityObject, EnableScreenReader, CurrentSettings.Accessibility.bEnableScreenReader);
    JSON_SET_BOOL(AccessibilityObject, ReducedMotion, CurrentSettings.Accessibility.bReducedMotion);
    JSON_SET_BOOL(AccessibilityObject, PhotosensitivityMode, CurrentSettings.Accessibility.bPhotosensitivityMode);
    RootObject->SetObjectField("Accessibility", AccessibilityObject);

    // NEW: Network
    TSharedPtr<FJsonObject> NetworkObject = MakeShareable(new FJsonObject);
    JSON_SET_FIELD(NetworkObject, MaxPingThreshold, CurrentSettings.Network.MaxPingThreshold);
    JSON_SET_FIELD(NetworkObject, NetworkSmoothing, CurrentSettings.Network.NetworkSmoothing);
    JSON_SET_FIELD(NetworkObject, BandwidthLimitKBps, CurrentSettings.Network.BandwidthLimitKBps);
    JSON_SET_STRING(NetworkObject, PreferredRegion, CurrentSettings.Network.PreferredRegion);
    JSON_SET_BOOL(NetworkObject, EnableCrossplay, CurrentSettings.Network.bEnableCrossplay);
    RootObject->SetObjectField("Network", NetworkObject);

    // NEW: Debug
    TSharedPtr<FJsonObject> DebugObject = MakeShareable(new FJsonObject);
    JSON_SET_BOOL(DebugObject, ShowPerformanceOverlay, CurrentSettings.Debug.bShowPerformanceOverlay);
    JSON_SET_BOOL(DebugObject, ShowNetworkStats, CurrentSettings.Debug.bShowNetworkStats);
    JSON_SET_BOOL(DebugObject, DeveloperMode, CurrentSettings.Debug.bDeveloperMode);
    JSON_SET_BOOL(DebugObject, EnableCrashReporting, CurrentSettings.Debug.bEnableCrashReporting);
    JSON_SET_BOOL(DebugObject, BenchmarkMode, CurrentSettings.Debug.bBenchmarkMode);
    RootObject->SetObjectField("Debug", DebugObject);

    return RootObject;
}

bool UUPMSettingsManager::JsonToSettings(TSharedPtr<FJsonObject> JsonObject)
{
    if (!JsonObject.IsValid())
    {
        return false;
    }

    // Graphics
    const TSharedPtr<FJsonObject>* GraphicsObject;
    if (JsonObject->TryGetObjectField("Graphics", GraphicsObject))
    {
        (*GraphicsObject)->TryGetNumberField("AntiAliasingQuality", CurrentSettings.Graphics.AntiAliasingQuality);
        (*GraphicsObject)->TryGetNumberField("ShadowQuality", CurrentSettings.Graphics.ShadowQuality);
        (*GraphicsObject)->TryGetNumberField("ViewDistanceQuality", CurrentSettings.Graphics.ViewDistanceQuality);
        (*GraphicsObject)->TryGetNumberField("PostProcessQuality", CurrentSettings.Graphics.PostProcessQuality);
        (*GraphicsObject)->TryGetNumberField("TextureQuality", CurrentSettings.Graphics.TextureQuality);
        (*GraphicsObject)->TryGetNumberField("EffectsQuality", CurrentSettings.Graphics.EffectsQuality);
        (*GraphicsObject)->TryGetNumberField("FoliageQuality", CurrentSettings.Graphics.FoliageQuality);
        (*GraphicsObject)->TryGetNumberField("ShadingQuality", CurrentSettings.Graphics.ShadingQuality);
    }

    // Rendering (EXPANDED)
    const TSharedPtr<FJsonObject>* RenderingObject;
    if (JsonObject->TryGetObjectField("Rendering", RenderingObject))
    {
        (*RenderingObject)->TryGetBoolField("EnableLumen", CurrentSettings.Rendering.bEnableLumen);
        (*RenderingObject)->TryGetBoolField("EnableRayTracing", CurrentSettings.Rendering.bEnableRayTracing);
        (*RenderingObject)->TryGetBoolField("EnableSSAO", CurrentSettings.Rendering.bEnableSSAO);
        (*RenderingObject)->TryGetBoolField("EnableSSR", CurrentSettings.Rendering.bEnableSSR);
        (*RenderingObject)->TryGetBoolField("EnableMotionBlur", CurrentSettings.Rendering.bEnableMotionBlur);
        (*RenderingObject)->TryGetBoolField("EnableBloom", CurrentSettings.Rendering.bEnableBloom);
        (*RenderingObject)->TryGetBoolField("EnableDepthOfField", CurrentSettings.Rendering.bEnableDepthOfField);
        (*RenderingObject)->TryGetBoolField("EnableLensFlares", CurrentSettings.Rendering.bEnableLensFlares);
        (*RenderingObject)->TryGetBoolField("EnableChromaticAberration", CurrentSettings.Rendering.bEnableChromaticAberration);
        (*RenderingObject)->TryGetBoolField("EnableFilmGrain", CurrentSettings.Rendering.bEnableFilmGrain);
        (*RenderingObject)->TryGetBoolField("EnableVignette", CurrentSettings.Rendering.bEnableVignette);
        (*RenderingObject)->TryGetBoolField("EnableVolumetricFog", CurrentSettings.Rendering.bEnableVolumetricFog);
        (*RenderingObject)->TryGetNumberField("AnisotropicFiltering", CurrentSettings.Rendering.AnisotropicFiltering);
        (*RenderingObject)->TryGetBoolField("EnableTAA", CurrentSettings.Rendering.bEnableTAA);
        int32 UpscalingInt = 0;
        (*RenderingObject)->TryGetNumberField("UpscalingMode", UpscalingInt);
        CurrentSettings.Rendering.UpscalingMode = static_cast<EUPMUpscalingMode>(UpscalingInt);
        (*RenderingObject)->TryGetNumberField("GlobalIlluminationQuality", CurrentSettings.Rendering.GlobalIlluminationQuality);
        (*RenderingObject)->TryGetNumberField("ReflectionQuality", CurrentSettings.Rendering.ReflectionQuality);
        (*RenderingObject)->TryGetBoolField("EnableSSGI", CurrentSettings.Rendering.bEnableSSGI);
        (*RenderingObject)->TryGetBoolField("EnableContactShadows", CurrentSettings.Rendering.bEnableContactShadows);
    }

    // Performance (EXPANDED)
    const TSharedPtr<FJsonObject>* PerformanceObject;
    if (JsonObject->TryGetObjectField("Performance", PerformanceObject))
    {
        (*PerformanceObject)->TryGetBoolField("EnableVSync", CurrentSettings.Performance.bEnableVSync);
        (*PerformanceObject)->TryGetNumberField("FrameRateLimit", CurrentSettings.Performance.FrameRateLimit);
        (*PerformanceObject)->TryGetBoolField("EnableDynamicResolution", CurrentSettings.Performance.bEnableDynamicResolution);
        (*PerformanceObject)->TryGetNumberField("MinFrameRateForDynamicRes", CurrentSettings.Performance.MinFrameRateForDynamicRes);
        (*PerformanceObject)->TryGetBoolField("EnableTripleBuffering", CurrentSettings.Performance.bEnableTripleBuffering);
        (*PerformanceObject)->TryGetBoolField("EnableAsyncCompute", CurrentSettings.Performance.bEnableAsyncCompute);
        (*PerformanceObject)->TryGetNumberField("LODDistanceMultiplier", CurrentSettings.Performance.LODDistanceMultiplier);
        (*PerformanceObject)->TryGetNumberField("ProcessPriority", CurrentSettings.Performance.ProcessPriority);
    }

    // Display (EXPANDED)
    const TSharedPtr<FJsonObject>* DisplayObject;
    if (JsonObject->TryGetObjectField("Display", DisplayObject))
    {
        int32 ResX = 1920, ResY = 1080;
        (*DisplayObject)->TryGetNumberField("ResolutionX", ResX);
        (*DisplayObject)->TryGetNumberField("ResolutionY", ResY);
        CurrentSettings.Display.Resolution = FIntPoint(ResX, ResY);

        int32 WindowModeInt = 0;
        (*DisplayObject)->TryGetNumberField("WindowMode", WindowModeInt);
        CurrentSettings.Display.WindowMode = static_cast<EWindowMode::Type>(WindowModeInt);

        (*DisplayObject)->TryGetNumberField("Brightness", CurrentSettings.Display.Brightness);
        (*DisplayObject)->TryGetNumberField("Contrast", CurrentSettings.Display.Contrast);
        (*DisplayObject)->TryGetBoolField("EnableHDR", CurrentSettings.Display.bEnableHDR);
        (*DisplayObject)->TryGetNumberField("HDRMaxNits", CurrentSettings.Display.HDRMaxNits);
        (*DisplayObject)->TryGetNumberField("MonitorIndex", CurrentSettings.Display.MonitorIndex);
        (*DisplayObject)->TryGetBoolField("BorderlessWindow", CurrentSettings.Display.bBorderlessWindow);
        (*DisplayObject)->TryGetNumberField("ScreenPercentage", CurrentSettings.Display.ScreenPercentage);
        (*DisplayObject)->TryGetNumberField("MenuFieldOfView", CurrentSettings.Display.MenuFieldOfView);
        (*DisplayObject)->TryGetNumberField("AspectRatioOverride", CurrentSettings.Display.AspectRatioOverride);
        (*DisplayObject)->TryGetNumberField("SafeZoneScale", CurrentSettings.Display.SafeZoneScale);
    }

    // Audio (EXPANDED)
    const TSharedPtr<FJsonObject>* AudioObject;
    if (JsonObject->TryGetObjectField("Audio", AudioObject))
    {
        (*AudioObject)->TryGetNumberField("MasterVolume", CurrentSettings.Audio.MasterVolume);
        (*AudioObject)->TryGetNumberField("SFXVolume", CurrentSettings.Audio.SFXVolume);
        (*AudioObject)->TryGetNumberField("MusicVolume", CurrentSettings.Audio.MusicVolume);
        (*AudioObject)->TryGetNumberField("VoiceDialogVolume", CurrentSettings.Audio.VoiceDialogVolume);
        (*AudioObject)->TryGetNumberField("AmbientVolume", CurrentSettings.Audio.AmbientVolume);
        (*AudioObject)->TryGetNumberField("UISoundVolume", CurrentSettings.Audio.UISoundVolume);
        (*AudioObject)->TryGetNumberField("VoiceChatVolume", CurrentSettings.Audio.VoiceChatVolume);
        (*AudioObject)->TryGetNumberField("AudioQuality", CurrentSettings.Audio.AudioQuality);
        (*AudioObject)->TryGetNumberField("SurroundSoundMode", CurrentSettings.Audio.SurroundSoundMode);
        (*AudioObject)->TryGetBoolField("EnableSpatialAudio", CurrentSettings.Audio.bEnableSpatialAudio);
        (*AudioObject)->TryGetNumberField("DynamicRange", CurrentSettings.Audio.DynamicRange);
        (*AudioObject)->TryGetNumberField("SubtitleTextSize", CurrentSettings.Audio.SubtitleTextSize);
        (*AudioObject)->TryGetNumberField("SubtitleBackgroundOpacity", CurrentSettings.Audio.SubtitleBackgroundOpacity);
    }

    // Gameplay (EXPANDED)
    const TSharedPtr<FJsonObject>* GameplayObject;
    if (JsonObject->TryGetObjectField("Gameplay", GameplayObject))
    {
        (*GameplayObject)->TryGetNumberField("FOV", CurrentSettings.Gameplay.FOV);
        (*GameplayObject)->TryGetNumberField("MouseSensitivity", CurrentSettings.Gameplay.MouseSensitivity);
        (*GameplayObject)->TryGetBoolField("InvertMouseY", CurrentSettings.Gameplay.bInvertMouseY);
        (*GameplayObject)->TryGetNumberField("ControllerSensitivity", CurrentSettings.Gameplay.ControllerSensitivity);
        (*GameplayObject)->TryGetNumberField("ControllerDeadZone", CurrentSettings.Gameplay.ControllerDeadZone);
        (*GameplayObject)->TryGetNumberField("AimAssistStrength", CurrentSettings.Gameplay.AimAssistStrength);
        (*GameplayObject)->TryGetNumberField("CameraShakeIntensity", CurrentSettings.Gameplay.CameraShakeIntensity);
        (*GameplayObject)->TryGetNumberField("HeadBobIntensity", CurrentSettings.Gameplay.HeadBobIntensity);
        (*GameplayObject)->TryGetBoolField("EnableVibration", CurrentSettings.Gameplay.bEnableVibration);
        (*GameplayObject)->TryGetBoolField("CrouchToggle", CurrentSettings.Gameplay.bCrouchToggle);
        (*GameplayObject)->TryGetBoolField("SprintToggle", CurrentSettings.Gameplay.bSprintToggle);
        (*GameplayObject)->TryGetBoolField("EnableAutoRun", CurrentSettings.Gameplay.bEnableAutoRun);
        (*GameplayObject)->TryGetNumberField("CameraSmoothing", CurrentSettings.Gameplay.CameraSmoothing);
    }

    // NEW: Accessibility
    const TSharedPtr<FJsonObject>* AccessibilityObject;
    if (JsonObject->TryGetObjectField("Accessibility", AccessibilityObject))
    {
        int32 ColorblindInt = 0;
        (*AccessibilityObject)->TryGetNumberField("ColorblindMode", ColorblindInt);
        CurrentSettings.Accessibility.ColorblindMode = static_cast<EUPMColorblindMode>(ColorblindInt);
        (*AccessibilityObject)->TryGetNumberField("UIScale", CurrentSettings.Accessibility.UIScale);
        (*AccessibilityObject)->TryGetNumberField("TextSize", CurrentSettings.Accessibility.TextSize);
        (*AccessibilityObject)->TryGetBoolField("HighContrastMode", CurrentSettings.Accessibility.bHighContrastMode);
        (*AccessibilityObject)->TryGetBoolField("EnableScreenReader", CurrentSettings.Accessibility.bEnableScreenReader);
        (*AccessibilityObject)->TryGetBoolField("ReducedMotion", CurrentSettings.Accessibility.bReducedMotion);
        (*AccessibilityObject)->TryGetBoolField("PhotosensitivityMode", CurrentSettings.Accessibility.bPhotosensitivityMode);
    }

    // NEW: Network
    const TSharedPtr<FJsonObject>* NetworkObject;
    if (JsonObject->TryGetObjectField("Network", NetworkObject))
    {
        (*NetworkObject)->TryGetNumberField("MaxPingThreshold", CurrentSettings.Network.MaxPingThreshold);
        (*NetworkObject)->TryGetNumberField("NetworkSmoothing", CurrentSettings.Network.NetworkSmoothing);
        (*NetworkObject)->TryGetNumberField("BandwidthLimitKBps", CurrentSettings.Network.BandwidthLimitKBps);
        (*NetworkObject)->TryGetStringField("PreferredRegion", CurrentSettings.Network.PreferredRegion);
        (*NetworkObject)->TryGetBoolField("EnableCrossplay", CurrentSettings.Network.bEnableCrossplay);
    }

    // NEW: Debug
    const TSharedPtr<FJsonObject>* DebugObject;
    if (JsonObject->TryGetObjectField("Debug", DebugObject))
    {
        (*DebugObject)->TryGetBoolField("ShowPerformanceOverlay", CurrentSettings.Debug.bShowPerformanceOverlay);
        (*DebugObject)->TryGetBoolField("ShowNetworkStats", CurrentSettings.Debug.bShowNetworkStats);
        (*DebugObject)->TryGetBoolField("DeveloperMode", CurrentSettings.Debug.bDeveloperMode);
        (*DebugObject)->TryGetBoolField("EnableCrashReporting", CurrentSettings.Debug.bEnableCrashReporting);
        (*DebugObject)->TryGetBoolField("BenchmarkMode", CurrentSettings.Debug.bBenchmarkMode);
    }

    return true;
}

#undef JSON_SET_FIELD
#undef JSON_SET_BOOL
#undef JSON_SET_STRING
