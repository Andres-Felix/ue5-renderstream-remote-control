/* ===========================
   UE5 Custom Device Controller
   For Moving Heads, RGB LEDs, Pyrotechnics, etc.
   =========================== */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "CustomDeviceController.generated.h"

// ===========================
// Device Type Enumerations
// ===========================

UENUM(BlueprintType)
enum class EDeviceType : uint8
{
    MovingHead UMETA(DisplayName = "Moving Head Light"),
    RGBLed UMETA(DisplayName = "RGB LED Strip"),
    Laser UMETA(DisplayName = "Laser Projector"),
    Pyrotechnics UMETA(DisplayName = "Pyrotechnics"),
    Fog UMETA(DisplayName = "Fog Machine"),
    Strobe UMETA(DisplayName = "Strobe Light"),
    ParCan UMETA(DisplayName = "Par Can Light"),
    MovingScrim UMETA(DisplayName = "Moving Scrim"),
    ConfettiCannon UMETA(DisplayName = "Confetti Cannon"),
    HazeGenerator UMETA(DisplayName = "Haze Generator"),
    Custom UMETA(DisplayName = "Custom Device")
};

// ===========================
// Device State Structures
// ===========================

USTRUCT(BlueprintType)
struct FMovingHeadState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Moving Head")
    float PanAngle = 0.0f; // 0-540 degrees

    UPROPERTY(BlueprintReadWrite, Category = "Moving Head")
    float TiltAngle = 0.0f; // 0-270 degrees

    UPROPERTY(BlueprintReadWrite, Category = "Moving Head")
    float Intensity = 0.0f; // 0-100%

    UPROPERTY(BlueprintReadWrite, Category = "Moving Head")
    FLinearColor Color = FLinearColor::White;

    UPROPERTY(BlueprintReadWrite, Category = "Moving Head")
    float Zoom = 1.0f; // 0.5-2.0

    UPROPERTY(BlueprintReadWrite, Category = "Moving Head")
    float Focus = 1.0f; // 0-1

    UPROPERTY(BlueprintReadWrite, Category = "Moving Head")
    float Strobe = 0.0f; // 0-100%

    UPROPERTY(BlueprintReadWrite, Category = "Moving Head")
    bool bChannelEnabled = true;
};

USTRUCT(BlueprintType)
struct FRGBLedState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "RGB LED")
    FLinearColor Color = FLinearColor::Red;

    UPROPERTY(BlueprintReadWrite, Category = "RGB LED")
    float Brightness = 1.0f; // 0-1

    UPROPERTY(BlueprintReadWrite, Category = "RGB LED")
    float Saturation = 1.0f; // 0-1

    UPROPERTY(BlueprintReadWrite, Category = "RGB LED")
    float AnimationSpeed = 1.0f; // 0-2

    UPROPERTY(BlueprintReadWrite, Category = "RGB LED")
    FString AnimationMode = TEXT("static"); // static, pulse, rainbow, chase, etc.

    UPROPERTY(BlueprintReadWrite, Category = "RGB LED")
    bool bFadeBetweenColors = true;
};

USTRUCT(BlueprintType)
struct FPyrotechnicsState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Pyrotechnics")
    bool bArmed = false;

    UPROPERTY(BlueprintReadWrite, Category = "Pyrotechnics")
    bool bTriggered = false;

    UPROPERTY(BlueprintReadWrite, Category = "Pyrotechnics")
    float Intensity = 1.0f; // 0-1 (effect strength)

    UPROPERTY(BlueprintReadWrite, Category = "Pyrotechnics")
    FString EffectType = TEXT("explosion"); // explosion, burst, fire, smoke

    UPROPERTY(BlueprintReadWrite, Category = "Pyrotechnics")
    float Duration = 1.0f; // effect duration in seconds

    UPROPERTY(BlueprintReadWrite, Category = "Pyrotechnics")
    FVector Location = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite, Category = "Pyrotechnics")
    bool bSafetyLocked = true; // prevents accidental trigger
};

USTRUCT(BlueprintType)
struct FFogMachineState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Fog Machine")
    bool bActive = false;

    UPROPERTY(BlueprintReadWrite, Category = "Fog Machine")
    float Density = 0.0f; // 0-1

    UPROPERTY(BlueprintReadWrite, Category = "Fog Machine")
    float Duration = 5.0f; // how long to emit

    UPROPERTY(BlueprintReadWrite, Category = "Fog Machine")
    FVector Location = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite, Category = "Fog Machine")
    float HeatingTime = 30.0f; // time to heat up
};

// ===========================
// Event Delegates
// ===========================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeviceStateChanged, FString, DeviceName, FString, StateJSON);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMovingHeadUpdated, FString, DeviceName, FMovingHeadState, State, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRGBLedUpdated, FString, DeviceName, FRGBLedState, State, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPyrotechnicsTriggered, FString, DeviceName, FPyrotechnicsState, State, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFogMachineTriggered, FString, DeviceName, FFogMachineState, State, bool, bSuccess);

/**
 * Custom Device Controller
 * Manages moving heads, RGB LEDs, pyrotechnics, and other stage equipment
 */
UCLASS()
class YOURPROJECT_API ACustomDeviceController : public AActor
{
    GENERATED_BODY()

public:
    ACustomDeviceController();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaTime) override;

    // ===========================
    // Device Registration
    // ===========================

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Setup")
    void RegisterDevice(FString DeviceName, EDeviceType DeviceType, int32 DMXChannel = 1);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Setup")
    void UnregisterDevice(FString DeviceName);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Setup")
    void UnregisterAllDevices();

    // ===========================
    // Moving Head Control
    // ===========================

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Moving Head")
    void SetMovingHeadPan(FString DeviceName, float PanAngle, float TransitionTime = 0.5f);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Moving Head")
    void SetMovingHeadTilt(FString DeviceName, float TiltAngle, float TransitionTime = 0.5f);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Moving Head")
    void SetMovingHeadColor(FString DeviceName, FLinearColor Color, float TransitionTime = 0.5f);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Moving Head")
    void SetMovingHeadIntensity(FString DeviceName, float Intensity, float TransitionTime = 0.5f);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Moving Head")
    void SetMovingHeadZoom(FString DeviceName, float Zoom, float TransitionTime = 0.5f);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Moving Head")
    void SetMovingHeadStrobe(FString DeviceName, float StrobeRate, float TransitionTime = 0.5f);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Moving Head")
    void SetMovingHeadFull(FString DeviceName, FMovingHeadState State, float TransitionTime = 0.5f);

    // ===========================
    // RGB LED Control
    // ===========================

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|RGB LED")
    void SetRGBLedColor(FString DeviceName, FLinearColor Color, float Brightness = 1.0f, float TransitionTime = 0.5f);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|RGB LED")
    void SetRGBLedAnimation(FString DeviceName, FString AnimationMode, float Speed = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|RGB LED")
    void SetRGBLedBrightness(FString DeviceName, float Brightness, float TransitionTime = 0.5f);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|RGB LED")
    void SetRGBLedFull(FString DeviceName, FRGBLedState State, float TransitionTime = 0.5f);

    // ===========================
    // Pyrotechnics Control
    // ===========================

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Pyrotechnics")
    void ArmPyrotechnics(FString DeviceName);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Pyrotechnics")
    void DisarmPyrotechnics(FString DeviceName);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Pyrotechnics")
    void TriggerPyrotechnics(FString DeviceName, FVector Location, FString EffectType = TEXT("explosion"), float Intensity = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Pyrotechnics")
    void SetPyrotechnicsFull(FString DeviceName, FPyrotechnicsState State);

    // ===========================
    // Fog Machine Control
    // ===========================

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Fog Machine")
    void ActivateFogMachine(FString DeviceName, float Density = 0.5f, float Duration = 5.0f);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Fog Machine")
    void DeactivateFogMachine(FString DeviceName);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Fog Machine")
    void SetFogMachineFull(FString DeviceName, FFogMachineState State);

    // ===========================
    // Generic Device Control
    // ===========================

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Generic")
    void SetGenericDeviceParameter(FString DeviceName, FString ParameterName, float Value);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Generic")
    void TriggerGenericEffect(FString DeviceName, FString EffectName);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Generic")
    void SendCustomCommand(FString DeviceName, FString Command, FString Parameters = TEXT(""));

    // ===========================
    // Group Operations
    // ===========================

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Groups")
    void CreateDeviceGroup(FString GroupName, TArray<FString> DeviceNames);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Groups")
    void SetGroupParameter(FString GroupName, FString ParameterName, float Value);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Groups")
    void TriggerGroupEffect(FString GroupName, FString EffectName);

    // ===========================
    // Preset System
    // ===========================

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Presets")
    void SavePreset(FString PresetName, FString PresetData);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Presets")
    void LoadPreset(FString PresetName);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Presets")
    void DeletePreset(FString PresetName);

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Presets")
    FString GetPresetList() const;

    // ===========================
    // Event Dispatchers
    // ===========================

    UPROPERTY(BlueprintAssignable, Category = "Custom Devices|Events")
    FOnDeviceStateChanged OnDeviceStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Custom Devices|Events")
    FOnMovingHeadUpdated OnMovingHeadUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Custom Devices|Events")
    FOnRGBLedUpdated OnRGBLedUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Custom Devices|Events")
    FOnPyrotechnicsTriggered OnPyrotechnicsTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Custom Devices|Events")
    FOnFogMachineTriggered OnFogMachineTriggered;

    // ===========================
    // Configuration
    // ===========================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Devices|Config")
    bool bEnableDMXOutput = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Devices|Config")
    int32 DMXUniverse = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Devices|Config")
    bool bSimulateDevices = true; // For testing without real hardware

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Devices|Config")
    float SimulationTickRate = 0.016f; // 60 FPS

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Devices|Config")
    bool bEnableLogging = true;

    // ===========================
    // State Queries
    // ===========================

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Query")
    FString GetDeviceState(FString DeviceName) const;

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Query")
    TArray<FString> GetAllDeviceNames() const;

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Query")
    FString GetAllDevicesState() const;

    UFUNCTION(BlueprintCallable, Category = "Custom Devices|Query")
    bool DeviceExists(FString DeviceName) const;

private:
    // Internal device storage
    struct FDevice
    {
        EDeviceType Type;
        int32 DMXChannel;
        FString Name;
        FMovingHeadState MovingHeadState;
        FRGBLedState RGBLedState;
        FPyrotechnicsState PyrotechnicsState;
        FFogMachineState FogMachineState;
        FString CustomState;
    };

    UPROPERTY()
    TMap<FString, FDevice> Devices;

    UPROPERTY()
    TMap<FString, TArray<FString>> DeviceGroups;

    UPROPERTY()
    TMap<FString, FString> PresetStorage;

    // Helper functions
    void HandleHTTPCommand(const FString& Command, const TSharedPtr<FJsonObject>& JsonObject);
    FLinearColor HexToColor(const FString& HexColor) const;
    FString ColorToHex(const FLinearColor& Color) const;
    void Log(const FString& Message) const;
};
