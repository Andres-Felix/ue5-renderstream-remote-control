/* ===========================
   UE5 Custom Device Controller
   C++ Implementation
   =========================== */

#include "CustomDeviceController.h"
#include "JsonObjectConverter.h"
#include "Misc/DefaultValueHelper.h"

ACustomDeviceController::ACustomDeviceController()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.016f; // 60 FPS

    bEnableDMXOutput = true;
    bSimulateDevices = true;
    bEnableLogging = true;
}

// ===========================
// Actor Lifecycle
// ===========================

void ACustomDeviceController::BeginPlay()
{
    Super::BeginPlay();
    Log("Custom Device Controller initialized");
}

void ACustomDeviceController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnregisterAllDevices();
    Super::EndPlay(EndPlayReason);
}

void ACustomDeviceController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Simulate device updates and transitions
    if (bSimulateDevices)
    {
        // Process device animations and state updates
        for (auto& DevicePair : Devices)
        {
            // Device-specific tick logic can be added here
        }
    }
}

// ===========================
// Device Registration
// ===========================

void ACustomDeviceController::RegisterDevice(FString DeviceName, EDeviceType DeviceType, int32 DMXChannel)
{
    if (DeviceExists(DeviceName))
    {
        Log(FString::Printf(TEXT("Device '%s' already registered!"), *DeviceName));
        return;
    }

    FDevice NewDevice;
    NewDevice.Name = DeviceName;
    NewDevice.Type = DeviceType;
    NewDevice.DMXChannel = DMXChannel;

    // Initialize default states based on device type
    switch (DeviceType)
    {
        case EDeviceType::MovingHead:
            NewDevice.MovingHeadState.bChannelEnabled = true;
            NewDevice.MovingHeadState.Intensity = 0.0f;
            NewDevice.MovingHeadState.Color = FLinearColor::White;
            break;

        case EDeviceType::RGBLed:
            NewDevice.RGBLedState.Color = FLinearColor::Red;
            NewDevice.RGBLedState.Brightness = 0.0f;
            NewDevice.RGBLedState.AnimationMode = TEXT("static");
            break;

        case EDeviceType::Pyrotechnics:
            NewDevice.PyrotechnicsState.bArmed = false;
            NewDevice.PyrotechnicsState.bSafetyLocked = true;
            break;

        case EDeviceType::Fog:
            NewDevice.FogMachineState.bActive = false;
            NewDevice.FogMachineState.Density = 0.0f;
            break;

        default:
            NewDevice.CustomState = TEXT("{}");
            break;
    }

    Devices.Add(DeviceName, NewDevice);
    
    FString TypeName = UEnum::GetValueAsString(DeviceType);
    Log(FString::Printf(TEXT("Device registered: '%s' (Type: %s, DMX: %d)"), 
        *DeviceName, *TypeName, DMXChannel));

    // Broadcast event
    OnDeviceStateChanged.Broadcast(DeviceName, TEXT("{\"status\": \"registered\"}"));
}

void ACustomDeviceController::UnregisterDevice(FString DeviceName)
{
    if (Devices.Contains(DeviceName))
    {
        Devices.Remove(DeviceName);
        Log(FString::Printf(TEXT("Device unregistered: '%s'"), *DeviceName));
        OnDeviceStateChanged.Broadcast(DeviceName, TEXT("{\"status\": \"unregistered\"}"));
    }
}

void ACustomDeviceController::UnregisterAllDevices()
{
    Devices.Empty();
    Log("All devices unregistered");
}

// ===========================
// Moving Head Control
// ===========================

void ACustomDeviceController::SetMovingHeadPan(FString DeviceName, float PanAngle, float TransitionTime)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::MovingHead) return;

    PanAngle = FMath::Clamp(PanAngle, -540.0f, 540.0f);
    Device.MovingHeadState.PanAngle = PanAngle;

    Log(FString::Printf(TEXT("Moving Head '%s' Pan: %.2f°"), *DeviceName, PanAngle));
    OnMovingHeadUpdated.Broadcast(DeviceName, Device.MovingHeadState, true);
}

void ACustomDeviceController::SetMovingHeadTilt(FString DeviceName, float TiltAngle, float TransitionTime)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::MovingHead) return;

    TiltAngle = FMath::Clamp(TiltAngle, 0.0f, 270.0f);
    Device.MovingHeadState.TiltAngle = TiltAngle;

    Log(FString::Printf(TEXT("Moving Head '%s' Tilt: %.2f°"), *DeviceName, TiltAngle));
    OnMovingHeadUpdated.Broadcast(DeviceName, Device.MovingHeadState, true);
}

void ACustomDeviceController::SetMovingHeadColor(FString DeviceName, FLinearColor Color, float TransitionTime)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::MovingHead) return;

    Device.MovingHeadState.Color = Color;

    Log(FString::Printf(TEXT("Moving Head '%s' Color: RGB(%.2f, %.2f, %.2f)"), 
        *DeviceName, Color.R, Color.G, Color.B));
    OnMovingHeadUpdated.Broadcast(DeviceName, Device.MovingHeadState, true);
}

void ACustomDeviceController::SetMovingHeadIntensity(FString DeviceName, float Intensity, float TransitionTime)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::MovingHead) return;

    Intensity = FMath::Clamp(Intensity, 0.0f, 100.0f);
    Device.MovingHeadState.Intensity = Intensity;

    Log(FString::Printf(TEXT("Moving Head '%s' Intensity: %.1f%%"), *DeviceName, Intensity));
    OnMovingHeadUpdated.Broadcast(DeviceName, Device.MovingHeadState, true);
}

void ACustomDeviceController::SetMovingHeadZoom(FString DeviceName, float Zoom, float TransitionTime)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::MovingHead) return;

    Zoom = FMath::Clamp(Zoom, 0.5f, 2.0f);
    Device.MovingHeadState.Zoom = Zoom;

    Log(FString::Printf(TEXT("Moving Head '%s' Zoom: %.2f"), *DeviceName, Zoom));
    OnMovingHeadUpdated.Broadcast(DeviceName, Device.MovingHeadState, true);
}

void ACustomDeviceController::SetMovingHeadStrobe(FString DeviceName, float StrobeRate, float TransitionTime)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::MovingHead) return;

    StrobeRate = FMath::Clamp(StrobeRate, 0.0f, 100.0f);
    Device.MovingHeadState.Strobe = StrobeRate;

    Log(FString::Printf(TEXT("Moving Head '%s' Strobe: %.1f%%"), *DeviceName, StrobeRate));
    OnMovingHeadUpdated.Broadcast(DeviceName, Device.MovingHeadState, true);
}

void ACustomDeviceController::SetMovingHeadFull(FString DeviceName, FMovingHeadState State, float TransitionTime)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::MovingHead) return;

    Device.MovingHeadState = State;

    Log(FString::Printf(TEXT("Moving Head '%s' Full Update: Pan=%.1f° Tilt=%.1f° Int=%.1f%%"), 
        *DeviceName, State.PanAngle, State.TiltAngle, State.Intensity));
    OnMovingHeadUpdated.Broadcast(DeviceName, Device.MovingHeadState, true);
}

// ===========================
// RGB LED Control
// ===========================

void ACustomDeviceController::SetRGBLedColor(FString DeviceName, FLinearColor Color, float Brightness, float TransitionTime)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::RGBLed) return;

    Device.RGBLedState.Color = Color;
    Device.RGBLedState.Brightness = FMath::Clamp(Brightness, 0.0f, 1.0f);

    Log(FString::Printf(TEXT("RGB LED '%s' Color: RGB(%.2f, %.2f, %.2f) Brightness: %.2f"), 
        *DeviceName, Color.R, Color.G, Color.B, Brightness));
    OnRGBLedUpdated.Broadcast(DeviceName, Device.RGBLedState, true);
}

void ACustomDeviceController::SetRGBLedAnimation(FString DeviceName, FString AnimationMode, float Speed)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::RGBLed) return;

    Device.RGBLedState.AnimationMode = AnimationMode;
    Device.RGBLedState.AnimationSpeed = FMath::Clamp(Speed, 0.1f, 2.0f);

    Log(FString::Printf(TEXT("RGB LED '%s' Animation: %s (Speed: %.2f)"), 
        *DeviceName, *AnimationMode, Speed));
    OnRGBLedUpdated.Broadcast(DeviceName, Device.RGBLedState, true);
}

void ACustomDeviceController::SetRGBLedBrightness(FString DeviceName, float Brightness, float TransitionTime)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::RGBLed) return;

    Device.RGBLedState.Brightness = FMath::Clamp(Brightness, 0.0f, 1.0f);

    Log(FString::Printf(TEXT("RGB LED '%s' Brightness: %.2f"), *DeviceName, Brightness));
    OnRGBLedUpdated.Broadcast(DeviceName, Device.RGBLedState, true);
}

void ACustomDeviceController::SetRGBLedFull(FString DeviceName, FRGBLedState State, float TransitionTime)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::RGBLed) return;

    Device.RGBLedState = State;

    Log(FString::Printf(TEXT("RGB LED '%s' Full Update: Color RGB(%.2f, %.2f, %.2f) Mode: %s"), 
        *DeviceName, State.Color.R, State.Color.G, State.Color.B, *State.AnimationMode));
    OnRGBLedUpdated.Broadcast(DeviceName, Device.RGBLedState, true);
}

// ===========================
// Pyrotechnics Control
// ===========================

void ACustomDeviceController::ArmPyrotechnics(FString DeviceName)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::Pyrotechnics) return;

    Device.PyrotechnicsState.bArmed = true;
    Device.PyrotechnicsState.bSafetyLocked = false;

    Log(FString::Printf(TEXT("Pyrotechnics '%s' ARMED - Safety locked: %s"), 
        *DeviceName, Device.PyrotechnicsState.bSafetyLocked ? TEXT("YES") : TEXT("NO")));
}

void ACustomDeviceController::DisarmPyrotechnics(FString DeviceName)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::Pyrotechnics) return;

    Device.PyrotechnicsState.bArmed = false;
    Device.PyrotechnicsState.bTriggered = false;
    Device.PyrotechnicsState.bSafetyLocked = true;

    Log(FString::Printf(TEXT("Pyrotechnics '%s' DISARMED - Safety locked: YES"), *DeviceName));
}

void ACustomDeviceController::TriggerPyrotechnics(FString DeviceName, FVector Location, FString EffectType, float Intensity)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::Pyrotechnics) return;

    if (!Device.PyrotechnicsState.bArmed || Device.PyrotechnicsState.bSafetyLocked)
    {
        Log(FString::Printf(TEXT("Pyrotechnics '%s' BLOCKED - Not armed or safety locked!"), *DeviceName));
        return;
    }

    Device.PyrotechnicsState.bTriggered = true;
    Device.PyrotechnicsState.Location = Location;
    Device.PyrotechnicsState.EffectType = EffectType;
    Device.PyrotechnicsState.Intensity = FMath::Clamp(Intensity, 0.0f, 1.0f);

    Log(FString::Printf(TEXT("Pyrotechnics '%s' TRIGGERED - Type: %s Location: (%.1f, %.1f, %.1f) Intensity: %.2f"), 
        *DeviceName, *EffectType, Location.X, Location.Y, Location.Z, Intensity));

    OnPyrotechnicsTriggered.Broadcast(DeviceName, Device.PyrotechnicsState, true);
}

void ACustomDeviceController::SetPyrotechnicsFull(FString DeviceName, FPyrotechnicsState State)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::Pyrotechnics) return;

    Device.PyrotechnicsState = State;

    Log(FString::Printf(TEXT("Pyrotechnics '%s' Full Update: Armed=%s Safety=%s Type=%s"), 
        *DeviceName, 
        State.bArmed ? TEXT("YES") : TEXT("NO"),
        State.bSafetyLocked ? TEXT("YES") : TEXT("NO"),
        *State.EffectType));
}

// ===========================
// Fog Machine Control
// ===========================

void ACustomDeviceController::ActivateFogMachine(FString DeviceName, float Density, float Duration)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::Fog) return;

    Device.FogMachineState.bActive = true;
    Device.FogMachineState.Density = FMath::Clamp(Density, 0.0f, 1.0f);
    Device.FogMachineState.Duration = FMath::Max(Duration, 0.1f);

    Log(FString::Printf(TEXT("Fog Machine '%s' ACTIVATED - Density: %.2f Duration: %.1fs"), 
        *DeviceName, Density, Duration));

    OnFogMachineTriggered.Broadcast(DeviceName, Device.FogMachineState, true);
}

void ACustomDeviceController::DeactivateFogMachine(FString DeviceName)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::Fog) return;

    Device.FogMachineState.bActive = false;

    Log(FString::Printf(TEXT("Fog Machine '%s' DEACTIVATED"), *DeviceName));
}

void ACustomDeviceController::SetFogMachineFull(FString DeviceName, FFogMachineState State)
{
    if (!Devices.Contains(DeviceName)) return;

    FDevice& Device = Devices[DeviceName];
    if (Device.Type != EDeviceType::Fog) return;

    Device.FogMachineState = State;

    Log(FString::Printf(TEXT("Fog Machine '%s' Full Update: Active=%s Density=%.2f Duration=%.1fs"), 
        *DeviceName, 
        State.bActive ? TEXT("YES") : TEXT("NO"),
        State.Density,
        State.Duration));
}

// ===========================
// Generic Device Control
// ===========================

void ACustomDeviceController::SetGenericDeviceParameter(FString DeviceName, FString ParameterName, float Value)
{
    if (!Devices.Contains(DeviceName)) return;

    Log(FString::Printf(TEXT("Device '%s' Parameter '%s': %.2f"), *DeviceName, *ParameterName, Value));
    OnDeviceStateChanged.Broadcast(DeviceName, FString::Printf(TEXT("{\"parameter\": \"%s\", \"value\": %.2f}"), *ParameterName, Value));
}

void ACustomDeviceController::TriggerGenericEffect(FString DeviceName, FString EffectName)
{
    if (!Devices.Contains(DeviceName)) return;

    Log(FString::Printf(TEXT("Device '%s' Effect '%s' triggered"), *DeviceName, *EffectName));
    OnDeviceStateChanged.Broadcast(DeviceName, FString::Printf(TEXT("{\"effect\": \"%s\"}"), *EffectName));
}

void ACustomDeviceController::SendCustomCommand(FString DeviceName, FString Command, FString Parameters)
{
    if (!Devices.Contains(DeviceName)) return;

    Log(FString::Printf(TEXT("Device '%s' Command '%s' with parameters: %s"), *DeviceName, *Command, *Parameters));
    OnDeviceStateChanged.Broadcast(DeviceName, FString::Printf(TEXT("{\"command\": \"%s\", \"parameters\": %s}"), *Command, *Parameters));
}

// ===========================
// Group Operations
// ===========================

void ACustomDeviceController::CreateDeviceGroup(FString GroupName, TArray<FString> DeviceNames)
{
    DeviceGroups.Add(GroupName, DeviceNames);
    Log(FString::Printf(TEXT("Device Group '%s' created with %d devices"), *GroupName, DeviceNames.Num()));
}

void ACustomDeviceController::SetGroupParameter(FString GroupName, FString ParameterName, float Value)
{
    if (!DeviceGroups.Contains(GroupName)) return;

    TArray<FString>& GroupDevices = DeviceGroups[GroupName];
    for (const FString& DeviceName : GroupDevices)
    {
        SetGenericDeviceParameter(DeviceName, ParameterName, Value);
    }

    Log(FString::Printf(TEXT("Group '%s' Parameter '%s' set to %.2f on %d devices"), 
        *GroupName, *ParameterName, Value, GroupDevices.Num()));
}

void ACustomDeviceController::TriggerGroupEffect(FString GroupName, FString EffectName)
{
    if (!DeviceGroups.Contains(GroupName)) return;

    TArray<FString>& GroupDevices = DeviceGroups[GroupName];
    for (const FString& DeviceName : GroupDevices)
    {
        TriggerGenericEffect(DeviceName, EffectName);
    }

    Log(FString::Printf(TEXT("Group '%s' Effect '%s' triggered on %d devices"), 
        *GroupName, *EffectName, GroupDevices.Num()));
}

// ===========================
// Preset System
// ===========================

void ACustomDeviceController::SavePreset(FString PresetName, FString PresetData)
{
    PresetStorage.Add(PresetName, PresetData);
    Log(FString::Printf(TEXT("Preset '%s' saved"), *PresetName));
}

void ACustomDeviceController::LoadPreset(FString PresetName)
{
    if (!PresetStorage.Contains(PresetName))
    {
        Log(FString::Printf(TEXT("Preset '%s' not found!"), *PresetName));
        return;
    }

    FString PresetData = PresetStorage[PresetName];
    Log(FString::Printf(TEXT("Preset '%s' loaded"), *PresetName));
}

void ACustomDeviceController::DeletePreset(FString PresetName)
{
    if (PresetStorage.Contains(PresetName))
    {
        PresetStorage.Remove(PresetName);
        Log(FString::Printf(TEXT("Preset '%s' deleted"), *PresetName));
    }
}

FString ACustomDeviceController::GetPresetList() const
{
    FString PresetList = TEXT("[");
    bool bFirst = true;

    for (const auto& Preset : PresetStorage)
    {
        if (!bFirst) PresetList += TEXT(", ");
        PresetList += FString::Printf(TEXT("\"%s\""), *Preset.Key);
        bFirst = false;
    }

    PresetList += TEXT("]");
    return PresetList;
}

// ===========================
// State Queries
// ===========================

FString ACustomDeviceController::GetDeviceState(FString DeviceName) const
{
    if (!Devices.Contains(DeviceName))
    {
        return TEXT("{\"error\": \"Device not found\"}");
    }

    const FDevice& Device = Devices[DeviceName];
    FString TypeName = UEnum::GetValueAsString(Device.Type);

    return FString::Printf(TEXT("{\"name\": \"%s\", \"type\": \"%s\", \"dmx_channel\": %d}"), 
        *DeviceName, *TypeName, Device.DMXChannel);
}

TArray<FString> ACustomDeviceController::GetAllDeviceNames() const
{
    TArray<FString> DeviceNames;
    Devices.GetKeys(DeviceNames);
    return DeviceNames;
}

FString ACustomDeviceController::GetAllDevicesState() const
{
    FString AllStates = TEXT("[");
    bool bFirst = true;

    for (const auto& DevicePair : Devices)
    {
        if (!bFirst) AllStates += TEXT(", ");
        AllStates += GetDeviceState(DevicePair.Key);
        bFirst = false;
    }

    AllStates += TEXT("]");
    return AllStates;
}

bool ACustomDeviceController::DeviceExists(FString DeviceName) const
{
    return Devices.Contains(DeviceName);
}

// ===========================
// Helper Functions
// ===========================

FLinearColor ACustomDeviceController::HexToColor(const FString& HexColor) const
{
    FString Hex = HexColor;
    
    if (Hex.StartsWith(TEXT("#")))
    {
        Hex = Hex.RightChop(1);
    }

    uint32 ColorValue = 0;
    if (Hex.Len() == 6)
    {
        sscanf_s(TCHAR_TO_ANSI(*Hex), "%x", &ColorValue);
    }

    uint8 R = (ColorValue >> 16) & 0xFF;
    uint8 G = (ColorValue >> 8) & 0xFF;
    uint8 B = ColorValue & 0xFF;

    return FLinearColor(R / 255.0f, G / 255.0f, B / 255.0f, 1.0f);
}

FString ACustomDeviceController::ColorToHex(const FLinearColor& Color) const
{
    uint8 R = FMath::Clamp((int32)(Color.R * 255.0f), 0, 255);
    uint8 G = FMath::Clamp((int32)(Color.G * 255.0f), 0, 255);
    uint8 B = FMath::Clamp((int32)(Color.B * 255.0f), 0, 255);

    return FString::Printf(TEXT("#%02X%02X%02X"), R, G, B);
}

void ACustomDeviceController::Log(const FString& Message) const
{
    if (bEnableLogging)
    {
        UE_LOG(LogTemp, Warning, TEXT("[CustomDevices] %s"), *Message);
    }
}
