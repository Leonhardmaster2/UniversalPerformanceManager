# Universal Performance Manager - Usage Examples

This document provides practical examples for integrating UPM into your Unreal Engine project.

## Table of Contents
- [C++ Examples](#c-examples)
- [Blueprint Examples](#blueprint-examples)
- [Common Use Cases](#common-use-cases)
- [Integration Patterns](#integration-patterns)

---

## C++ Examples

### Example 1: Basic Setup in Game Instance

```cpp
// MyGameInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UPMSettingsManager.h"
#include "MyGameInstance.generated.h"

UCLASS()
class MYGAME_API UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

private:
    UPROPERTY()
    UUPMSettingsManager* SettingsManager;
};

// MyGameInstance.cpp
#include "MyGameInstance.h"

void UMyGameInstance::Init()
{
    Super::Init();

    // Get UPM manager and initialize
    SettingsManager = UUPMSettingsManager::GetInstance(this);
    if (SettingsManager)
    {
        // This loads settings from Saved/UPM/Settings.json
        // and applies them immediately
        SettingsManager->Initialize();

        UE_LOG(LogTemp, Log, TEXT("UPM Settings initialized successfully"));
    }
}
```

### Example 2: Performance HUD in Player Controller

```cpp
// MyPlayerController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UPMSettingsManager.h"
#include "MyPlayerController.generated.h"

UCLASS()
class MYGAME_API AMyPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void PlayerTick(float DeltaTime) override;

    // Toggle performance overlay with a key
    virtual void SetupInputComponent() override;

    UFUNCTION()
    void TogglePerformanceOverlay();

private:
    UPROPERTY()
    UUPMSettingsManager* SettingsManager;

    bool bShowPerformanceHUD;
};

// MyPlayerController.cpp
#include "MyPlayerController.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"

void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SettingsManager = UUPMSettingsManager::GetInstance(this);
    bShowPerformanceHUD = false;
}

void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Bind F1 key to toggle performance overlay
    InputComponent->BindAction("TogglePerformanceHUD", IE_Pressed, this,
        &AMyPlayerController::TogglePerformanceOverlay);
}

void AMyPlayerController::TogglePerformanceOverlay()
{
    bShowPerformanceHUD = !bShowPerformanceHUD;
}

void AMyPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    if (!SettingsManager)
        return;

    // Update performance metrics every frame
    SettingsManager->UpdatePerformanceMetrics(DeltaTime);

    if (bShowPerformanceHUD)
    {
        FUPMPerformanceMetrics Metrics = SettingsManager->GetPerformanceMetrics();

        // Display on HUD (simple debug text version)
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Green,
                FString::Printf(TEXT("FPS: %.1f (Avg: %.1f, Min: %.1f, Max: %.1f)"),
                    Metrics.FPS_Current, Metrics.FPS_Average,
                    Metrics.FPS_Min, Metrics.FPS_Max));

            GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::Yellow,
                FString::Printf(TEXT("CPU: %.2fms | GPU: %.2fms"),
                    Metrics.CPUFrameTime, Metrics.GPUFrameTime));

            GEngine->AddOnScreenDebugMessage(3, 0.0f, FColor::Cyan,
                FString::Printf(TEXT("RAM: %.0fMB | VRAM: %.0fMB"),
                    Metrics.RAMUsageMB, Metrics.VRAMUsageMB));
        }
    }
}
```

### Example 3: Settings Menu in C++

```cpp
// SettingsMenuWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPMSettingsManager.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "SettingsMenuWidget.generated.h"

UCLASS()
class MYGAME_API USettingsMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    // Graphics Quality Sliders
    UPROPERTY(meta = (BindWidget))
    USlider* AntiAliasingSlider;

    UPROPERTY(meta = (BindWidget))
    USlider* ShadowQualitySlider;

    // VSync Checkbox
    UPROPERTY(meta = (BindWidget))
    UCheckBox* VSyncCheckBox;

    // Resolution Dropdown
    UPROPERTY(meta = (BindWidget))
    UComboBoxString* ResolutionComboBox;

    UFUNCTION()
    void OnAntiAliasingChanged(float Value);

    UFUNCTION()
    void OnShadowQualityChanged(float Value);

    UFUNCTION()
    void OnVSyncChanged(bool bIsChecked);

    UFUNCTION()
    void OnResolutionSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnSaveButtonClicked();

    UFUNCTION()
    void OnApplyButtonClicked();

private:
    UPROPERTY()
    UUPMSettingsManager* SettingsManager;

    void LoadCurrentSettings();
};

// SettingsMenuWidget.cpp
#include "SettingsMenuWidget.h"

void USettingsMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SettingsManager = UUPMSettingsManager::GetInstance(this);

    // Bind events
    if (AntiAliasingSlider)
        AntiAliasingSlider->OnValueChanged.AddDynamic(this,
            &USettingsMenuWidget::OnAntiAliasingChanged);

    if (ShadowQualitySlider)
        ShadowQualitySlider->OnValueChanged.AddDynamic(this,
            &USettingsMenuWidget::OnShadowQualityChanged);

    if (VSyncCheckBox)
        VSyncCheckBox->OnCheckStateChanged.AddDynamic(this,
            &USettingsMenuWidget::OnVSyncChanged);

    if (ResolutionComboBox)
        ResolutionComboBox->OnSelectionChanged.AddDynamic(this,
            &USettingsMenuWidget::OnResolutionSelected);

    // Populate resolution dropdown
    if (ResolutionComboBox)
    {
        ResolutionComboBox->AddOption(TEXT("1280x720"));
        ResolutionComboBox->AddOption(TEXT("1920x1080"));
        ResolutionComboBox->AddOption(TEXT("2560x1440"));
        ResolutionComboBox->AddOption(TEXT("3840x2160"));
    }

    // Load current settings
    LoadCurrentSettings();
}

void USettingsMenuWidget::LoadCurrentSettings()
{
    if (!SettingsManager)
        return;

    FUPMCompleteSettings Settings = SettingsManager->GetAllSettings();

    // Update UI to match current settings
    if (AntiAliasingSlider)
        AntiAliasingSlider->SetValue(Settings.Graphics.AntiAliasingQuality);

    if (ShadowQualitySlider)
        ShadowQualitySlider->SetValue(Settings.Graphics.ShadowQuality);

    if (VSyncCheckBox)
        VSyncCheckBox->SetIsChecked(Settings.Performance.bEnableVSync);

    // Set resolution dropdown
    if (ResolutionComboBox)
    {
        FString ResolutionString = FString::Printf(TEXT("%dx%d"),
            Settings.Display.Resolution.X, Settings.Display.Resolution.Y);
        ResolutionComboBox->SetSelectedOption(ResolutionString);
    }
}

void USettingsMenuWidget::OnAntiAliasingChanged(float Value)
{
    if (SettingsManager)
    {
        SettingsManager->SetAntiAliasingQuality(FMath::RoundToInt(Value));
    }
}

void USettingsMenuWidget::OnShadowQualityChanged(float Value)
{
    if (SettingsManager)
    {
        SettingsManager->SetShadowQuality(FMath::RoundToInt(Value));
    }
}

void USettingsMenuWidget::OnVSyncChanged(bool bIsChecked)
{
    if (SettingsManager)
    {
        SettingsManager->SetVSyncEnabled(bIsChecked);
    }
}

void USettingsMenuWidget::OnResolutionSelected(FString SelectedItem,
    ESelectInfo::Type SelectionType)
{
    if (!SettingsManager)
        return;

    // Parse resolution string "1920x1080"
    FString WidthStr, HeightStr;
    if (SelectedItem.Split(TEXT("x"), &WidthStr, &HeightStr))
    {
        int32 Width = FCString::Atoi(*WidthStr);
        int32 Height = FCString::Atoi(*HeightStr);
        SettingsManager->SetResolution(FIntPoint(Width, Height));
    }
}

void USettingsMenuWidget::OnSaveButtonClicked()
{
    if (SettingsManager)
    {
        SettingsManager->SaveSettings();
        // Show confirmation message
    }
}

void USettingsMenuWidget::OnApplyButtonClicked()
{
    if (SettingsManager)
    {
        SettingsManager->ApplyAllSettings();
    }
}
```

---

## Blueprint Examples

### Example 1: Simple Performance Overlay Widget

**Blueprint: BP_PerformanceOverlay** (Parent: UPMPerformanceOverlayWidget)

**Designer:**
- Add Text blocks: FPSText, CPUTimeText, GPUTimeText, RAMText, VRAMText

**Event Graph:**

```
Event On Performance Metrics Updated
├─ Format Text: "FPS: {0}" with Metrics.FPS_Current
│  └─ Set Text (FPSText)
├─ Format Text: "CPU: {0}ms" with Metrics.CPUFrameTime
│  └─ Set Text (CPUTimeText)
├─ Format Text: "GPU: {0}ms" with Metrics.GPUFrameTime
│  └─ Set Text (GPUTimeText)
├─ Format Text: "RAM: {0}MB" with Metrics.RAMUsageMB
│  └─ Set Text (RAMText)
└─ Format Text: "VRAM: {0}MB" with Metrics.VRAMUsageMB
   └─ Set Text (VRAMText)
```

**Add to HUD:**
```
PlayerController → Event BeginPlay
└─ Create Widget: BP_PerformanceOverlay
   └─ Add to Viewport
```

### Example 2: Complete Settings Menu

**Blueprint: BP_SettingsMenu** (Parent: UPMSettingsPanelWidget)

**Designer:**
- Sliders: AA_Slider, Shadow_Slider, Texture_Slider
- Checkboxes: VSync_Checkbox, Lumen_Checkbox, RayTracing_Checkbox
- Buttons: Save_Button, Apply_Button, Reset_Button

**Event Graph:**

```
Event On Settings Loaded
├─ Set Value: AA_Slider → Settings.Graphics.AntiAliasingQuality
├─ Set Value: Shadow_Slider → Settings.Graphics.ShadowQuality
├─ Set Checked: VSync_Checkbox → Settings.Performance.bEnableVSync
└─ Set Checked: Lumen_Checkbox → Settings.Rendering.bEnableLumen

AA_Slider → On Value Changed
└─ SetAntiAliasingQuality(Value)

Shadow_Slider → On Value Changed
└─ SetShadowQuality(Value)

VSync_Checkbox → On Check State Changed
└─ SetVSyncEnabled(IsChecked)

Lumen_Checkbox → On Check State Changed
└─ SetLumenEnabled(IsChecked)

Save_Button → On Clicked
└─ SaveSettings
   └─ Branch (Success)
      ├─ True: Show "Settings Saved" message
      └─ False: Show "Failed to save" error

Apply_Button → On Clicked
└─ ApplySettings

Reset_Button → On Clicked
└─ ResetToDefaults
   └─ RefreshFromSettings
```

---

## Common Use Cases

### Use Case 1: Quality Presets

```cpp
void UMySettingsManager::SetQualityPreset(EQualityPreset Preset)
{
    UUPMSettingsManager* Manager = UUPMSettingsManager::GetInstance(this);
    if (!Manager)
        return;

    FUPMGraphicsSettings Graphics;

    switch (Preset)
    {
    case EQualityPreset::Low:
        Graphics.AntiAliasingQuality = 0;
        Graphics.ShadowQuality = 0;
        Graphics.ViewDistanceQuality = 0;
        Graphics.PostProcessQuality = 0;
        Graphics.TextureQuality = 0;
        break;

    case EQualityPreset::Medium:
        Graphics.AntiAliasingQuality = 2;
        Graphics.ShadowQuality = 2;
        Graphics.ViewDistanceQuality = 2;
        Graphics.PostProcessQuality = 2;
        Graphics.TextureQuality = 2;
        break;

    case EQualityPreset::High:
        Graphics.AntiAliasingQuality = 3;
        Graphics.ShadowQuality = 3;
        Graphics.ViewDistanceQuality = 3;
        Graphics.PostProcessQuality = 3;
        Graphics.TextureQuality = 3;
        break;

    case EQualityPreset::Ultra:
        Graphics.AntiAliasingQuality = 4;
        Graphics.ShadowQuality = 4;
        Graphics.ViewDistanceQuality = 4;
        Graphics.PostProcessQuality = 4;
        Graphics.TextureQuality = 4;
        break;
    }

    Manager->SetGraphicsSettings(Graphics);
    Manager->ApplyAllSettings();
}
```

### Use Case 2: FPS Counter Toggle

```cpp
// Add to Player Controller
void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindKey(EKeys::F1, IE_Pressed, this,
        &AMyPlayerController::ToggleFPSCounter);
}

void AMyPlayerController::ToggleFPSCounter()
{
    // Toggle your performance overlay widget visibility
    if (PerformanceOverlayWidget)
    {
        PerformanceOverlayWidget->ToggleOverlay();
    }
}
```

### Use Case 3: Auto-detect Best Settings

```cpp
void UMySettingsManager::AutoDetectSettings()
{
    UUPMSettingsManager* Manager = UUPMSettingsManager::GetInstance(this);
    if (!Manager)
        return;

    // Get system specs
    FPlatformMemoryStats MemoryStats = FPlatformMemory::GetStats();
    uint64 TotalRAM = MemoryStats.TotalPhysical / (1024 * 1024 * 1024); // GB

    // Determine quality based on RAM (simple example)
    int32 QualityLevel = 2; // Medium default

    if (TotalRAM >= 16)
        QualityLevel = 4; // Ultra
    else if (TotalRAM >= 8)
        QualityLevel = 3; // High
    else if (TotalRAM >= 4)
        QualityLevel = 2; // Medium
    else
        QualityLevel = 1; // Low

    // Apply settings
    Manager->SetAntiAliasingQuality(QualityLevel);
    Manager->SetShadowQuality(QualityLevel);
    Manager->SetViewDistanceQuality(QualityLevel);
    Manager->SetPostProcessQuality(QualityLevel);
    Manager->SetTextureQuality(QualityLevel);

    Manager->ApplyAllSettings();
    Manager->SaveSettings();
}
```

---

## Integration Patterns

### Pattern 1: Game Instance Initialization

```cpp
// Initialize UPM when the game starts
void UMyGameInstance::Init()
{
    Super::Init();

    UUPMSettingsManager* Manager = UUPMSettingsManager::GetInstance(this);
    if (Manager)
    {
        Manager->Initialize(); // Loads and applies saved settings
    }
}
```

### Pattern 2: Main Menu Integration

```cpp
// Main menu opens settings panel
void UMainMenuWidget::OnSettingsButtonClicked()
{
    // Create and show settings panel widget
    UUPMSettingsPanelWidget* SettingsPanel = CreateWidget<UUPMSettingsPanelWidget>(
        GetOwningPlayer(), SettingsPanelClass);

    if (SettingsPanel)
    {
        SettingsPanel->AddToViewport();
    }
}
```

### Pattern 3: In-Game Options Menu

```cpp
// Pause menu with settings access
void AMyPlayerController::TogglePauseMenu()
{
    if (!bPauseMenuOpen)
    {
        // Create pause menu with settings tab
        UPauseMenuWidget* PauseMenu = CreateWidget<UPauseMenuWidget>(
            this, PauseMenuClass);
        PauseMenu->AddToViewport();

        SetPause(true);
        SetShowMouseCursor(true);
        bPauseMenuOpen = true;
    }
    else
    {
        // Close and save settings
        UUPMSettingsManager* Manager = UUPMSettingsManager::GetInstance(this);
        if (Manager)
        {
            Manager->SaveSettings();
        }

        SetPause(false);
        SetShowMouseCursor(false);
        bPauseMenuOpen = false;
    }
}
```

---

## Best Practices

1. **Initialize Early**: Call `Initialize()` in your Game Instance's `Init()` function
2. **Save on Apply**: Call `SaveSettings()` when users click "Apply" or "OK"
3. **Update Regularly**: Call `UpdatePerformanceMetrics()` every frame for accurate data
4. **Configurable Overlay**: Make the performance overlay toggleable (F1, F2, etc.)
5. **Quality Presets**: Provide Low/Medium/High/Ultra presets for user convenience
6. **Visual Feedback**: Show confirmation messages when settings are saved
7. **Reset Option**: Always provide a way to reset to default settings

---

## Troubleshooting Examples

### Issue: Settings not persisting between sessions

**Solution:**
```cpp
// Ensure SaveSettings is called before application quit
void UMyGameInstance::Shutdown()
{
    UUPMSettingsManager* Manager = UUPMSettingsManager::GetInstance(this);
    if (Manager)
    {
        Manager->SaveSettings();
    }

    Super::Shutdown();
}
```

### Issue: Performance metrics showing as 0

**Solution:**
```cpp
// Make sure UpdatePerformanceMetrics is called every frame
void AMyHUD::DrawHUD()
{
    Super::DrawHUD();

    UUPMSettingsManager* Manager = UUPMSettingsManager::GetInstance(this);
    if (Manager)
    {
        Manager->UpdatePerformanceMetrics(GetWorld()->GetDeltaSeconds());
    }
}
```

---

**For more information, see the main README.md**
