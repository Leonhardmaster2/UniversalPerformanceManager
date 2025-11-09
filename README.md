# Universal Performance Manager (UPM Settings Core)

**A complete runtime settings and performance monitoring system for Unreal Engine that works in packaged builds.**

## Features

### ✨ Performance Overlay
- **Real-time FPS metrics**: Current, Average, Min, Max
- **Frame time monitoring**: CPU and GPU frame times
- **Memory tracking**: VRAM and RAM usage
- **Rendering statistics**: Draw calls, primitive count
- **Thread load monitoring**: Game, Render, and RHI threads

### ⚙️ Runtime Settings Management
- **Graphics settings**: Anti-aliasing, shadows, view distance, post-process, texture quality
- **Rendering features**: Lumen, ray tracing, SSAO, SSR, motion blur, bloom
- **Performance controls**: VSync, frame rate limiting
- **Display options**: Resolution, window mode
- **Audio controls**: Master, SFX, and Music volume
- **Gameplay settings**: FOV, mouse sensitivity

### 💾 Persistent Storage
- Settings automatically saved to `Saved/UPM/Settings.json`
- Auto-loads on game startup
- Works in packaged builds (Steam, itch.io, consoles)
- No engine config modifications required

### 🚀 Instant Application
- All settings apply **immediately without restart**
- Uses `UGameUserSettings`, console variables, and JSON
- Build-stable and production-ready

## Installation

### For Plugin Use
1. Copy the `UniversalPerformanceManager` folder to your project's `Plugins/` directory
2. Right-click your `.uproject` file and select "Generate Visual Studio project files"
3. Open your project in Unreal Engine
4. Enable the plugin in Edit → Plugins → Universal Performance Manager

### For Module Integration
1. Copy the `Source/UniversalPerformanceManager` folder to your project's `Source/` directory
2. Add `"UniversalPerformanceManager"` to your project's `.uproject` or `.Build.cs` module dependencies
3. Regenerate project files

## Quick Start

### C++ Usage

```cpp
#include "UPMSettingsManager.h"

// Get the manager instance
UUPMSettingsManager* Manager = UUPMSettingsManager::GetInstance(this);

// Update performance metrics (call every frame)
void AMyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (Manager)
    {
        Manager->UpdatePerformanceMetrics(DeltaTime);
        FUPMPerformanceMetrics Metrics = Manager->GetPerformanceMetrics();
        // Use metrics for display
    }
}

// Change settings
Manager->SetAntiAliasingQuality(3);
Manager->SetShadowQuality(2);
Manager->SetVSyncEnabled(false);
Manager->SetFrameRateLimit(144.0f);

// Save settings
Manager->SaveSettings();

// Load settings
Manager->LoadSettings();
```

### Blueprint Usage

#### Performance Overlay Widget

1. Create a new Widget Blueprint inheriting from `UPMPerformanceOverlayWidget`
2. Design your overlay UI (text blocks for FPS, frame time, etc.)
3. Override the `OnPerformanceMetricsUpdated` event
4. Bind the metrics to your UI elements

Example Blueprint setup:
```
OnPerformanceMetricsUpdated(Metrics)
├─ Set FPS Text = "FPS: {Metrics.FPS_Current}"
├─ Set CPU Time Text = "CPU: {Metrics.CPUFrameTime}ms"
├─ Set GPU Time Text = "GPU: {Metrics.GPUFrameTime}ms"
└─ Set VRAM Text = "VRAM: {Metrics.VRAMUsageMB}MB"
```

4. Add the widget to your viewport:
```
Event BeginPlay
└─ Create Widget (YourPerformanceOverlayWidget)
   └─ Add to Viewport
```

#### Settings Panel Widget

1. Create a new Widget Blueprint inheriting from `UPMSettingsPanelWidget`
2. Design your settings UI (sliders, checkboxes, dropdowns)
3. Wire up UI controls to the provided functions:
   - `SetAntiAliasingQuality`
   - `SetShadowQuality`
   - `SetVSyncEnabled`
   - etc.
4. Implement `OnSettingsLoaded` to initialize UI from loaded settings
5. Call `SaveSettings` when user clicks "Apply" or "Save"

Example Blueprint setup:
```
AntiAliasingSlider → OnValueChanged
└─ SetAntiAliasingQuality(Value)

VSyncCheckbox → OnCheckStateChanged
└─ SetVSyncEnabled(IsChecked)

SaveButton → OnClicked
└─ SaveSettings
   └─ ApplySettings
```

## API Reference

### UUPMSettingsManager

#### Singleton Access
```cpp
static UUPMSettingsManager* GetInstance(const UObject* WorldContextObject)
```

#### Performance Monitoring
```cpp
void UpdatePerformanceMetrics(float DeltaTime)
FUPMPerformanceMetrics GetPerformanceMetrics() const
void ResetPerformanceStats()
```

#### Graphics Settings
```cpp
void SetAntiAliasingQuality(int32 Quality)      // 0-4
void SetShadowQuality(int32 Quality)            // 0-4
void SetViewDistanceQuality(int32 Quality)      // 0-4
void SetPostProcessQuality(int32 Quality)       // 0-4
void SetTextureQuality(int32 Quality)           // 0-4
```

#### Rendering Settings
```cpp
void SetLumenEnabled(bool bEnabled)
void SetRayTracingEnabled(bool bEnabled)
void SetSSAOEnabled(bool bEnabled)
void SetSSREnabled(bool bEnabled)
```

#### Performance Settings
```cpp
void SetVSyncEnabled(bool bEnabled)
void SetFrameRateLimit(float Limit)             // 0 = unlimited
```

#### Display Settings
```cpp
void SetResolution(FIntPoint Resolution)
void SetWindowMode(TEnumAsByte<EWindowMode::Type> Mode)
```

#### Audio Settings
```cpp
void SetMasterVolume(float Volume)              // 0.0 - 1.0
void SetSFXVolume(float Volume)                 // 0.0 - 1.0
void SetMusicVolume(float Volume)               // 0.0 - 1.0
```

#### Gameplay Settings
```cpp
void SetFOV(float FOV)                          // 60.0 - 120.0
void SetMouseSensitivity(float Sensitivity)     // 0.1 - 5.0
```

#### Persistence
```cpp
bool SaveSettings()
bool LoadSettings()
void Initialize()                               // Auto-load and apply
```

### Widget Classes

#### UPMPerformanceOverlayWidget
Base class for performance overlay UI
- Automatically updates metrics
- Toggle visibility
- Blueprint event: `OnPerformanceMetricsUpdated`

#### UPMSettingsPanelWidget
Base class for settings panel UI
- Easy access to all settings functions
- Save/Load functionality
- Blueprint event: `OnSettingsLoaded`

## Settings File Format

Settings are saved as JSON in `Saved/UPM/Settings.json`:

```json
{
  "Graphics": {
    "AntiAliasingQuality": 3,
    "ShadowQuality": 3,
    "ViewDistanceQuality": 3,
    "PostProcessQuality": 3,
    "TextureQuality": 3,
    "EffectsQuality": 3,
    "FoliageQuality": 3,
    "ShadingQuality": 3
  },
  "Rendering": {
    "EnableLumen": true,
    "EnableRayTracing": false,
    "EnableSSAO": true,
    "EnableSSR": true,
    "EnableMotionBlur": true,
    "EnableBloom": true
  },
  "Performance": {
    "EnableVSync": true,
    "FrameRateLimit": 0.0
  },
  "Display": {
    "ResolutionX": 1920,
    "ResolutionY": 1080,
    "WindowMode": 0
  },
  "Audio": {
    "MasterVolume": 1.0,
    "SFXVolume": 1.0,
    "MusicVolume": 0.8
  },
  "Gameplay": {
    "FOV": 90.0,
    "MouseSensitivity": 1.0,
    "InvertMouseY": false
  }
}
```

## Architecture

### No Engine Modifications
UPM uses only official Unreal Engine APIs:
- `UGameUserSettings` for scalability settings
- Console variables (`sg.*`, `r.*`, `t.*`) for advanced rendering
- JSON for custom gameplay settings
- Standard file I/O for persistence

### Build Stability
All features work in:
- ✅ Editor builds
- ✅ Development builds
- ✅ Shipping builds
- ✅ Steam deployments
- ✅ itch.io deployments
- ✅ Console platforms (with appropriate platform support)

## Customization

### Custom Settings Categories
Extend the system with your own settings:

```cpp
// Add to UPMSettingsManager.h
USTRUCT(BlueprintType)
struct FMyCustomSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    float MyCustomValue;
};

// Add to FUPMCompleteSettings
UPROPERTY(BlueprintReadWrite)
FMyCustomSettings Custom;
```

### Custom Performance Metrics
Add game-specific metrics:

```cpp
// Extend FUPMPerformanceMetrics
UPROPERTY(BlueprintReadOnly)
int32 MyGameSpecificStat;
```

## Performance Considerations

- **Minimal overhead**: Performance monitoring uses efficient stat gathering
- **Configurable update rate**: Control overlay update frequency
- **Lazy loading**: Settings only load when needed
- **Efficient JSON**: Fast serialization with cached objects

## Troubleshooting

### Settings not persisting
- Check that `Saved/UPM/` directory has write permissions
- Verify `SaveSettings()` is being called
- Check logs for error messages

### Settings not applying
- Ensure `ApplyAllSettings()` or specific apply functions are called
- Some settings require specific Unreal Engine features to be enabled
- Check console for CVar availability

### Performance metrics showing zeros
- Make sure `UpdatePerformanceMetrics(DeltaTime)` is called every frame
- Verify the manager instance is valid
- Some metrics require specific engine features

## License

Copyright Universal Performance Manager. All Rights Reserved.

## Support

For issues, questions, or feature requests, please refer to the project repository.

---

**Built with ❤️ for the Unreal Engine community**

*Make your games look great and run smoothly on every platform.*
