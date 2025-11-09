// Copyright Universal Performance Manager. All Rights Reserved.

#include "UPMSettingsManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HAL/PlatformMemory.h"
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

    // Get GPU time if available
    if (GEngine && GEngine->GetEngineSubsystem<UObject>())
    {
        // GPU time estimation (approximate)
        PerformanceMetrics.GPUFrameTime = DeltaTime * 1000.0f * 0.8f; // Rough estimate
    }

    // Memory usage
    FPlatformMemoryStats MemoryStats = FPlatformMemory::GetStats();
    PerformanceMetrics.RAMUsageMB = static_cast<float>(MemoryStats.UsedPhysical) / (1024.0f * 1024.0f);

    // VRAM usage (if available)
    if (GDynamicRHI)
    {
        uint64 TotalVRAM = 0;
        uint64 UsedVRAM = 0;

        // Try to get VRAM info
        RHIGetResourceMemoryUsage(UsedVRAM);

        if (UsedVRAM > 0)
        {
            PerformanceMetrics.VRAMUsageMB = static_cast<float>(UsedVRAM) / (1024.0f * 1024.0f);
        }
    }

    // Draw calls and primitive count (approximate from stats)
    #if STATS
    if (GEngine)
    {
        // These are rough estimates - actual values would need stat system integration
        PerformanceMetrics.DrawCalls = 1000; // Placeholder
        PerformanceMetrics.PrimitiveCount = 50000; // Placeholder
    }
    #endif

    // Thread load (normalized 0-1)
    PerformanceMetrics.GameThreadLoad = FMath::Clamp(DeltaTime / 0.0166f, 0.0f, 1.0f); // 60 FPS baseline
    PerformanceMetrics.RenderThreadLoad = PerformanceMetrics.GameThreadLoad * 0.9f; // Estimate
    PerformanceMetrics.RHIThreadLoad = PerformanceMetrics.GameThreadLoad * 0.7f; // Estimate
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

// ==================== Rendering Settings ====================

void UUPMSettingsManager::SetRenderingSettings(const FUPMRenderingSettings& Settings)
{
    CurrentSettings.Rendering = Settings;
    ApplyRenderingSettings();
}

void UUPMSettingsManager::SetLumenEnabled(bool bEnabled)
{
    CurrentSettings.Rendering.bEnableLumen = bEnabled;
    ApplyRenderingSettings();
}

void UUPMSettingsManager::SetRayTracingEnabled(bool bEnabled)
{
    CurrentSettings.Rendering.bEnableRayTracing = bEnabled;
    ApplyRenderingSettings();
}

void UUPMSettingsManager::SetSSAOEnabled(bool bEnabled)
{
    CurrentSettings.Rendering.bEnableSSAO = bEnabled;
    ApplyRenderingSettings();
}

void UUPMSettingsManager::SetSSREnabled(bool bEnabled)
{
    CurrentSettings.Rendering.bEnableSSR = bEnabled;
    ApplyRenderingSettings();
}

void UUPMSettingsManager::ApplyRenderingSettings()
{
    if (!GEngine)
    {
        return;
    }

    // Apply console variables for rendering features
    IConsoleVariable* CVarLumen = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.DiffuseIndirect.Allow"));
    if (CVarLumen)
    {
        CVarLumen->Set(CurrentSettings.Rendering.bEnableLumen ? 1 : 0);
    }

    IConsoleVariable* CVarRayTracing = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing"));
    if (CVarRayTracing)
    {
        CVarRayTracing->Set(CurrentSettings.Rendering.bEnableRayTracing ? 1 : 0);
    }

    IConsoleVariable* CVarSSAO = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientOcclusionLevels"));
    if (CVarSSAO)
    {
        CVarSSAO->Set(CurrentSettings.Rendering.bEnableSSAO ? 3 : 0);
    }

    IConsoleVariable* CVarSSR = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SSR.Quality"));
    if (CVarSSR)
    {
        CVarSSR->Set(CurrentSettings.Rendering.bEnableSSR ? 3 : 0);
    }

    IConsoleVariable* CVarMotionBlur = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MotionBlurQuality"));
    if (CVarMotionBlur)
    {
        CVarMotionBlur->Set(CurrentSettings.Rendering.bEnableMotionBlur ? 4 : 0);
    }

    IConsoleVariable* CVarBloom = IConsoleManager::Get().FindConsoleVariable(TEXT("r.BloomQuality"));
    if (CVarBloom)
    {
        CVarBloom->Set(CurrentSettings.Rendering.bEnableBloom ? 5 : 0);
    }
}

// ==================== Performance Settings ====================

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

void UUPMSettingsManager::ApplyPerformanceSettings()
{
    UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings();
    if (!GameSettings)
    {
        return;
    }

    // Apply VSync
    GameSettings->SetVSyncEnabled(CurrentSettings.Performance.bEnableVSync);

    // Apply frame rate limit
    if (CurrentSettings.Performance.FrameRateLimit > 0.0f)
    {
        GameSettings->SetFrameRateLimit(CurrentSettings.Performance.FrameRateLimit);
    }
    else
    {
        GameSettings->SetFrameRateLimit(0.0f); // Unlimited
    }

    // Also set via console variables for immediate effect
    IConsoleVariable* CVarVSync = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSync"));
    if (CVarVSync)
    {
        CVarVSync->Set(CurrentSettings.Performance.bEnableVSync ? 1 : 0);
    }

    IConsoleVariable* CVarMaxFPS = IConsoleManager::Get().FindConsoleVariable(TEXT("t.MaxFPS"));
    if (CVarMaxFPS)
    {
        CVarMaxFPS->Set(CurrentSettings.Performance.FrameRateLimit);
    }

    GameSettings->ApplySettings(false);
}

// ==================== Display Settings ====================

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

void UUPMSettingsManager::ApplyDisplaySettings()
{
    UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings();
    if (!GameSettings)
    {
        return;
    }

    // Set resolution
    GameSettings->SetScreenResolution(CurrentSettings.Display.Resolution);

    // Set window mode
    GameSettings->SetFullscreenMode(CurrentSettings.Display.WindowMode);

    // Apply immediately
    GameSettings->ApplySettings(false);
}

// ==================== Audio Settings ====================

void UUPMSettingsManager::SetAudioSettings(const FUPMAudioSettings& Settings)
{
    CurrentSettings.Audio = Settings;
    ApplyAudioSettings();
}

void UUPMSettingsManager::SetMasterVolume(float Volume)
{
    CurrentSettings.Audio.MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    ApplyAudioSettings();
}

void UUPMSettingsManager::SetSFXVolume(float Volume)
{
    CurrentSettings.Audio.SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    ApplyAudioSettings();
}

void UUPMSettingsManager::SetMusicVolume(float Volume)
{
    CurrentSettings.Audio.MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    ApplyAudioSettings();
}

void UUPMSettingsManager::ApplyAudioSettings()
{
    // Note: Audio application depends on your audio system
    // This is a placeholder for common audio system integration

    // For Sound Classes or Audio Mixer, you would use:
    // USoundClass* MasterSoundClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/Audio/MasterSoundClass"));
    // if (MasterSoundClass)
    // {
    //     UGameplayStatics::SetSoundClassVolume(this, MasterSoundClass, CurrentSettings.Audio.MasterVolume);
    // }

    // Or use console variables if your project uses them
    IConsoleVariable* CVarMasterVolume = IConsoleManager::Get().FindConsoleVariable(TEXT("au.MasterVolume"));
    if (CVarMasterVolume)
    {
        CVarMasterVolume->Set(CurrentSettings.Audio.MasterVolume);
    }
}

// ==================== Gameplay Settings ====================

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

void UUPMSettingsManager::ApplyGameplaySettings()
{
    // Gameplay settings are typically applied through game-specific systems
    // You would implement this based on your game's architecture
    // For example:
    // - FOV: Applied to camera component
    // - Mouse Sensitivity: Applied to player controller input

    // This is where you would notify your game systems about the changes
}

// ==================== Persistence ====================

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

    // Create directory if it doesn't exist
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*DirectoryPath))
    {
        if (!PlatformFile.CreateDirectoryTree(*DirectoryPath))
        {
            UE_LOG(LogTemp, Error, TEXT("UPM: Failed to create directory: %s"), *DirectoryPath);
            return false;
        }
    }

    // Write to file
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

TSharedPtr<FJsonObject> UUPMSettingsManager::SettingsToJson() const
{
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);

    // Graphics
    TSharedPtr<FJsonObject> GraphicsObject = MakeShareable(new FJsonObject);
    GraphicsObject->SetNumberField("AntiAliasingQuality", CurrentSettings.Graphics.AntiAliasingQuality);
    GraphicsObject->SetNumberField("ShadowQuality", CurrentSettings.Graphics.ShadowQuality);
    GraphicsObject->SetNumberField("ViewDistanceQuality", CurrentSettings.Graphics.ViewDistanceQuality);
    GraphicsObject->SetNumberField("PostProcessQuality", CurrentSettings.Graphics.PostProcessQuality);
    GraphicsObject->SetNumberField("TextureQuality", CurrentSettings.Graphics.TextureQuality);
    GraphicsObject->SetNumberField("EffectsQuality", CurrentSettings.Graphics.EffectsQuality);
    GraphicsObject->SetNumberField("FoliageQuality", CurrentSettings.Graphics.FoliageQuality);
    GraphicsObject->SetNumberField("ShadingQuality", CurrentSettings.Graphics.ShadingQuality);
    RootObject->SetObjectField("Graphics", GraphicsObject);

    // Rendering
    TSharedPtr<FJsonObject> RenderingObject = MakeShareable(new FJsonObject);
    RenderingObject->SetBoolField("EnableLumen", CurrentSettings.Rendering.bEnableLumen);
    RenderingObject->SetBoolField("EnableRayTracing", CurrentSettings.Rendering.bEnableRayTracing);
    RenderingObject->SetBoolField("EnableSSAO", CurrentSettings.Rendering.bEnableSSAO);
    RenderingObject->SetBoolField("EnableSSR", CurrentSettings.Rendering.bEnableSSR);
    RenderingObject->SetBoolField("EnableMotionBlur", CurrentSettings.Rendering.bEnableMotionBlur);
    RenderingObject->SetBoolField("EnableBloom", CurrentSettings.Rendering.bEnableBloom);
    RootObject->SetObjectField("Rendering", RenderingObject);

    // Performance
    TSharedPtr<FJsonObject> PerformanceObject = MakeShareable(new FJsonObject);
    PerformanceObject->SetBoolField("EnableVSync", CurrentSettings.Performance.bEnableVSync);
    PerformanceObject->SetNumberField("FrameRateLimit", CurrentSettings.Performance.FrameRateLimit);
    RootObject->SetObjectField("Performance", PerformanceObject);

    // Display
    TSharedPtr<FJsonObject> DisplayObject = MakeShareable(new FJsonObject);
    DisplayObject->SetNumberField("ResolutionX", CurrentSettings.Display.Resolution.X);
    DisplayObject->SetNumberField("ResolutionY", CurrentSettings.Display.Resolution.Y);
    DisplayObject->SetNumberField("WindowMode", static_cast<int32>(CurrentSettings.Display.WindowMode));
    RootObject->SetObjectField("Display", DisplayObject);

    // Audio
    TSharedPtr<FJsonObject> AudioObject = MakeShareable(new FJsonObject);
    AudioObject->SetNumberField("MasterVolume", CurrentSettings.Audio.MasterVolume);
    AudioObject->SetNumberField("SFXVolume", CurrentSettings.Audio.SFXVolume);
    AudioObject->SetNumberField("MusicVolume", CurrentSettings.Audio.MusicVolume);
    RootObject->SetObjectField("Audio", AudioObject);

    // Gameplay
    TSharedPtr<FJsonObject> GameplayObject = MakeShareable(new FJsonObject);
    GameplayObject->SetNumberField("FOV", CurrentSettings.Gameplay.FOV);
    GameplayObject->SetNumberField("MouseSensitivity", CurrentSettings.Gameplay.MouseSensitivity);
    GameplayObject->SetBoolField("InvertMouseY", CurrentSettings.Gameplay.bInvertMouseY);
    RootObject->SetObjectField("Gameplay", GameplayObject);

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

    // Rendering
    const TSharedPtr<FJsonObject>* RenderingObject;
    if (JsonObject->TryGetObjectField("Rendering", RenderingObject))
    {
        (*RenderingObject)->TryGetBoolField("EnableLumen", CurrentSettings.Rendering.bEnableLumen);
        (*RenderingObject)->TryGetBoolField("EnableRayTracing", CurrentSettings.Rendering.bEnableRayTracing);
        (*RenderingObject)->TryGetBoolField("EnableSSAO", CurrentSettings.Rendering.bEnableSSAO);
        (*RenderingObject)->TryGetBoolField("EnableSSR", CurrentSettings.Rendering.bEnableSSR);
        (*RenderingObject)->TryGetBoolField("EnableMotionBlur", CurrentSettings.Rendering.bEnableMotionBlur);
        (*RenderingObject)->TryGetBoolField("EnableBloom", CurrentSettings.Rendering.bEnableBloom);
    }

    // Performance
    const TSharedPtr<FJsonObject>* PerformanceObject;
    if (JsonObject->TryGetObjectField("Performance", PerformanceObject))
    {
        (*PerformanceObject)->TryGetBoolField("EnableVSync", CurrentSettings.Performance.bEnableVSync);
        (*PerformanceObject)->TryGetNumberField("FrameRateLimit", CurrentSettings.Performance.FrameRateLimit);
    }

    // Display
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
    }

    // Audio
    const TSharedPtr<FJsonObject>* AudioObject;
    if (JsonObject->TryGetObjectField("Audio", AudioObject))
    {
        (*AudioObject)->TryGetNumberField("MasterVolume", CurrentSettings.Audio.MasterVolume);
        (*AudioObject)->TryGetNumberField("SFXVolume", CurrentSettings.Audio.SFXVolume);
        (*AudioObject)->TryGetNumberField("MusicVolume", CurrentSettings.Audio.MusicVolume);
    }

    // Gameplay
    const TSharedPtr<FJsonObject>* GameplayObject;
    if (JsonObject->TryGetObjectField("Gameplay", GameplayObject))
    {
        (*GameplayObject)->TryGetNumberField("FOV", CurrentSettings.Gameplay.FOV);
        (*GameplayObject)->TryGetNumberField("MouseSensitivity", CurrentSettings.Gameplay.MouseSensitivity);
        (*GameplayObject)->TryGetBoolField("InvertMouseY", CurrentSettings.Gameplay.bInvertMouseY);
    }

    return true;
}
