# Custom Devices Controller Setup Guide

## 📋 Overview

The Custom Devices Controller allows you to manage stage equipment and props from your UE5 project, including:

- **Moving Head Lights** - Pan, tilt, color, intensity, zoom, strobe
- **RGB LED Strips** - Color control, animations, brightness
- **Pyrotechnics** - Safe armed/disarmed triggering with safety locks
- **Fog Machines** - Density and duration control
- **Generic Devices** - Any custom equipment with parameters
- **Device Groups** - Control multiple devices simultaneously
- **Presets** - Save and load device configurations

---

## 🔧 Step 1: Add C++ to Your Project

### Copy Files
```
YourProject/Source/YourProject/
├── CustomDeviceController.h
└── CustomDeviceController.cpp
```

### Update Build Configuration

Edit `YourProject.Build.cs`:

```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core", "CoreUObject", "Engine", "InputCore",
    "HTTP", "Json", "JsonUtilities", "HttpServer"
});
```

### Compile
- **Visual Studio**: Build → Rebuild Solution
- **UE5 Editor**: Tools → Compile

---

## 🎮 Step 2: Create Blueprint Controller

### In UE5 Editor

1. **Content Browser** → **+ New Blueprint Class**
2. Search for **CustomDeviceController**
3. Create and name it: `BP_CustomDevices`
4. Configure in Details:
   ```
   - Enable DMX Output: True (if using DMX hardware)
   - Simulate Devices: True (for testing)
   - Enable Logging: True (for debugging)
   ```

### Add to Level

1. Open your level
2. Drag `BP_CustomDevices` into the viewport
3. Save the level

---

## 📡 Step 3: Device Registration

### In Level Blueprint

Create a function or event to register devices:

```blueprint
Event BeginPlay
├─ Get BP_CustomDevices (CustomDevices variable)
├─ Register Device
│   ├─ Device Name: "Moving Head 1"
│   ├─ Device Type: Moving Head Light
│   ├─ DMX Channel: 1
│
├─ Register Device
│   ├─ Device Name: "RGB LED 1"
│   ├─ Device Type: RGB LED Strip
│   ├─ DMX Channel: 10
│
├─ Register Device
│   ├─ Device Name: "Fog Machine"
│   ├─ Device Type: Fog Machine
│   ├─ DMX Channel: 20
│
└─ Register Device
    ├─ Device Name: "Pyrotechnics"
    ├─ Device Type: Pyrotechnics
    ├─ DMX Channel: 30
```

---

## ☀️ Step 4: Moving Head Light Setup

### Physical Setup
- Mount moving head light in your scene
- Position as a Static Mesh or Light Actor

### Blueprint Integration

```blueprint
Create Custom Event: UpdateMovingHead
├─ Input: HeadName (String), Pan (Float), Tilt (Float), Color (LinearColor)
├─ Get Moving Head Actor (by tag or variable)
├─ Set Actor Rotation
│   └─ From Pan/Tilt angles
├─ Set Light Color
│   └─ From Color parameter
└─ Set Light Intensity
    └─ Based on intensity parameter
```

### Connect Event Dispatcher

```blueprint
BP_CustomDevices → On Moving Head Updated
├─ Call UpdateMovingHead
├─ Parameters:
│   ├─ Device Name
│   ├─ Pan Angle
│   ├─ Tilt Angle
│   └─ Color
```

### XY Pad Control (Web UI)

The custom devices panel includes an interactive XY pad for precise pan/tilt control:
- Click anywhere on the pad to set position
- Visual indicator shows current position
- Real-time updates to the moving head

---

## 🎨 Step 5: RGB LED Strip Setup

### Material Setup

1. Create **Material**: `M_LEDStrip`
2. Add parameters:
   ```
   - Base Color (Vector3)
   - Brightness (Scalar 0-1)
   - Animation Mode (String)
   - Animation Speed (Scalar)
   ```

### Blueprint Integration

```blueprint
Create Custom Event: UpdateRGBLED
├─ Input: LEDName (String), Color (LinearColor), Brightness (Float)
├─ Get LED Static Mesh Actor
├─ Create Dynamic Material Instance
├─ Set Vector Parameter "Base Color"
├─ Set Scalar Parameter "Brightness"
└─ Handle Animation Mode
    ├─ Static: Keep color
    ├─ Pulse: Lerp brightness
    ├─ Rainbow: Cycle colors
    ├─ Chase: Animate across LEDs
    └─ Strobe: Blink rapidly
```

### Connect Event Dispatcher

```blueprint
BP_CustomDevices → On RGB LED Updated
├─ Call UpdateRGBLED
├─ Parameters:
│   ├─ Device Name
│   ├─ Color
│   ├─ Brightness
│   └─ Animation Mode
```

### Animation Modes

| Mode | Effect |
|------|--------|
| **static** | Solid color, no animation |
| **pulse** | Fade in/out with current color |
| **rainbow** | Cycle through color spectrum |
| **chase** | Color runs across the strip |
| **strobe** | Rapid on/off blinking |

---

## 💥 Step 6: Pyrotechnics Setup

### Safety Features

The system includes multiple safety layers:
1. **Armed State** - Device must be armed before triggering
2. **Safety Lock** - Prevents accidental trigger
3. **Unlock Sequence** - Two-step arming process

### Blueprint Integration

```blueprint
Create Custom Event: TriggerPyroEffect
├─ Input: EffectName (String), Location (Vector), Intensity (Float)
├─ Create Particle System
│   ├─ Based on EffectName
│   ├─ Spawn at Location
│   └─ Set Intensity Scale
├─ Play Sound Effect
├─ Add Camera Shake (for impact)
└─ Optional: Apply Physical Force
```

### Effect Types

```
- explosion: Large blast with particles and shake
- burst: Multiple small bursts
- fire: Flame effect with light
- smoke: Smoke cloud without blast
- spark: Sparkling cascade
```

### Connect Event Dispatcher

```blueprint
BP_CustomDevices → On Pyrotechnics Triggered
├─ Check Armed Status
├─ Check Safety Lock
├─ Call TriggerPyroEffect
├─ Parameters:
│   ├─ Device Name
│   ├─ Effect Type
│   ├─ Location
│   └─ Intensity
```

### Web UI Controls

```
1. Arm Button - Enables the device
2. Disarm Button - Disables and locks
3. Effect Type Dropdown - Select effect
4. Intensity Slider - Adjust strength
5. TRIGGER Button - Execute (only when armed)
```

---

## ☁️ Step 7: Fog Machine Setup

### Setup Fog Actor

1. Create **Fog Effect Particle System**
2. Blueprint: `BP_FogVolume`
3. Properties:
   ```
   - Spawn Rate: Controlled by density
   - Lifetime: Based on duration
   - Velocity: Upward spread
   - Size: Scales with intensity
   ```

### Blueprint Integration

```blueprint
Create Custom Event: ActivateFog
├─ Input: Density (Float 0-1), Duration (Float seconds)
├─ Get Fog Volume Actor
├─ Set Particle Spawn Rate
│   └─ Density * MaxSpawnRate
├─ Activate Particle System
├─ Wait Duration seconds
└─ Deactivate Particle System (optional)
```

### Connect Event Dispatcher

```blueprint
BP_CustomDevices → On Fog Machine Triggered
├─ Call ActivateFog
├─ Parameters:
│   ├─ Device Name
│   ├─ Density
│   └─ Duration
```

---

## 👥 Step 8: Device Groups

### Create a Group

```blueprint
Create Device Group
├─ Group Name: "Stage Lights"
├─ Devices:
│   ├─ "Moving Head 1"
│   ├─ "Moving Head 2"
│   ├─ "RGB LED 1"
│   └─ "Strobe Light"
```

### Control Group

```blueprint
Set Group Parameter
├─ Group Name: "Stage Lights"
├─ Parameter: "intensity"
├─ Value: 75.0
└─ Result: All lights set to 75% intensity
```

### Use Cases

- **Spotlight Group**: Multiple moving heads
- **Ambient Lighting**: All LED strips
- **Effect Group**: Pyrotechnics + Fog + Strobes

---

## 💾 Step 9: Preset System

### Save Preset

```blueprint
Custom Event: SaveScenePreset
├─ Collect all device states
├─ Create JSON:
│   {
│     "moving_heads": [
│       {"name": "Head 1", "pan": 180, "tilt": 45, "intensity": 100}
│     ],
│     "rgb_leds": [
│       {"name": "LED 1", "color": "#ff0000", "brightness": 80}
│     ],
│     "timestamp": "2024-06-12T15:30:00Z"
│   }
└─ Save Preset with name
```

### Load Preset

```blueprint
Custom Event: LoadScenePreset
├─ Get Preset by Name
├─ For Each Device in Preset
│   ├─ Apply saved values
│   ├─ Animate transitions
│   └─ Update UI
```

### Common Presets

| Preset | Use Case |
|--------|----------|
| **Intro** | Stage setup, ambient lighting |
| **Performance** | Bright, dynamic lighting |
| **Spotlight** | Focused moving head on performer |
| **Strobe** | High energy, dance floor |
| **Blackout** | All lights off |
| **Emergency** | Safety lighting |

---

## 🌐 Step 10: Web Control Panel Access

### Access the Panel

**Local:**
```
http://localhost:3000/devices.html
```

**Network:**
```
http://<your-machine-ip>:3000/devices.html
```

### Web UI Features

#### Sidebar
- Device list with selection
- Visual indicator for device type
- Quick access to all devices

#### Control Cards
- **Moving Head**: XY pad, pan/tilt/color/intensity/zoom/strobe
- **RGB LED**: Color picker, brightness, animation modes
- **Pyrotechnics**: Arm/disarm, effect type, intensity, trigger
- **Fog Machine**: Activate/deactivate, density, duration
- **Generic**: Parameter and effect inputs

#### Real-time Updates
- Live connection status
- Instant device feedback
- Error notifications
- Success confirmations

---

## 🧪 Testing & Troubleshooting

### Test Individual Devices

1. **Moving Head**:
   - Move pan/tilt sliders
   - Change color with picker
   - Verify light rotates in scene

2. **RGB LED**:
   - Select different colors
   - Toggle animation modes
   - Watch LED update

3. **Fog**:
   - Activate fog machine
   - Adjust density
   - See fog particles in scene

4. **Pyrotechnics**:
   - Test Arm button (should enable controls)
   - Trigger effect and verify particles
   - Test Disarm to lock

### Common Issues

#### Problem: "Device not registered"
**Solution:**
- Verify device is registered in Level Blueprint
- Check device name matches exactly
- Ensure BP_CustomDevices is in level

#### Problem: "No visual feedback"
**Solution:**
- Enable logging in BP_CustomDevices
- Check Output Log for errors
- Verify actors have correct tags
- Test with simulation mode first

#### Problem: "Web panel not loading"
**Solution:**
- Verify server is running
- Check port 3000 is open
- Try direct IP instead of localhost
- Check Windows Firewall

---

## 📊 DMX Integration

### Enable DMX Output

1. In `BP_CustomDevices` Details:
   ```
   Enable DMX Output: True
   DMX Universe: 1
   ```

2. Devices automatically map to DMX channels:
   ```
   Device → DMX Channel → Physical Equipment
   ```

### DMX Channel Mapping

```
Universe 1:
├─ Channels 1-6: Moving Head 1
├─ Channels 7-9: RGB LED 1
├─ Channels 10-15: Moving Head 2
└─ Channels 16-18: RGB LED 2
```

### Compatible Equipment

- **Moving Heads**: ETC Source Four, Clay Paky Sharpy
- **LED Strips**: Chauvet COLORband, ADJ RGB
- **Fog Machines**: Chauvet Hurricane, American DJ Mammoth
- **Pyrotechnics**: PyroLink systems

---

## 🎬 Advanced Features

### Custom Device Types

Extend the system with custom devices:

```cpp
// Add to CustomDeviceController.h
UENUM(BlueprintType)
enum class EDeviceType : uint8
{
    // ... existing types ...
    CustomProjector UMETA(DisplayName = "Custom Projector"),
    SoundSystem UMETA(DisplayName = "Sound System"),
};
```

### Scripting & Automation

Create complex lighting sequences:

```blueprint
Timeline: "Opening Sequence"
├─ T=0s: Blackout (all off)
├─ T=2s: Fog activate
├─ T=3s: Moving heads position
├─ T=4s: RGB LEDs to blue
├─ T=5s: Strobe begins
├─ T=10s: Full intensity
├─ T=15s: Pyro trigger
└─ T=20s: Transition to main scene
```

---

## 📞 API Reference

### Blueprint Callables

```cpp
// Registration
RegisterDevice(DeviceName, DeviceType, DMXChannel)
UnregisterDevice(DeviceName)
UnregisterAllDevices()

// Moving Head
SetMovingHeadPan(DeviceName, PanAngle, TransitionTime)
SetMovingHeadTilt(DeviceName, TiltAngle, TransitionTime)
SetMovingHeadColor(DeviceName, Color, TransitionTime)
SetMovingHeadIntensity(DeviceName, Intensity, TransitionTime)
SetMovingHeadZoom(DeviceName, Zoom, TransitionTime)
SetMovingHeadStrobe(DeviceName, StrobeRate, TransitionTime)
SetMovingHeadFull(DeviceName, FullState, TransitionTime)

// RGB LED
SetRGBLedColor(DeviceName, Color, Brightness, TransitionTime)
SetRGBLedAnimation(DeviceName, AnimationMode, Speed)
SetRGBLedBrightness(DeviceName, Brightness, TransitionTime)
SetRGBLedFull(DeviceName, FullState, TransitionTime)

// Pyrotechnics
ArmPyrotechnics(DeviceName)
DisarmPyrotechnics(DeviceName)
TriggerPyrotechnics(DeviceName, Location, EffectType, Intensity)
SetPyrotechnicsFull(DeviceName, FullState)

// Fog Machine
ActivateFogMachine(DeviceName, Density, Duration)
DeactivateFogMachine(DeviceName)
SetFogMachineFull(DeviceName, FullState)

// Generic
SetGenericDeviceParameter(DeviceName, ParameterName, Value)
TriggerGenericEffect(DeviceName, EffectName)
SendCustomCommand(DeviceName, Command, Parameters)

// Groups
CreateDeviceGroup(GroupName, DeviceNames)
SetGroupParameter(GroupName, ParameterName, Value)
TriggerGroupEffect(GroupName, EffectName)

// Presets
SavePreset(PresetName, PresetData)
LoadPreset(PresetName)
DeletePreset(PresetName)
GetPresetList()
```

### Event Dispatchers

```cpp
OnDeviceStateChanged(DeviceName, StateJSON)
OnMovingHeadUpdated(DeviceName, State, bSuccess)
OnRGBLedUpdated(DeviceName, State, bSuccess)
OnPyrotechnicsTriggered(DeviceName, State, bSuccess)
OnFogMachineTriggered(DeviceName, State, bSuccess)
```

---

## 🎯 Quick Start Checklist

- [ ] Copy C++ files to project
- [ ] Update `.Build.cs` with module dependencies
- [ ] Compile project
- [ ] Create `BP_CustomDevices` blueprint
- [ ] Add to level
- [ ] Register devices in Level Blueprint
- [ ] Setup event dispatchers
- [ ] Create visual representations (actors/materials)
- [ ] Test web panel at `/devices.html`
- [ ] Test each device type individually
- [ ] Setup presets for common scenes
- [ ] Configure DMX if using hardware
- [ ] Deploy and enjoy! 🎉

---

## 📖 Example Setup: Concert Stage

```blueprint
// BeginPlay Event

// Register all devices
RegisterDevice("Main Head 1", Moving Head Light, 1)
RegisterDevice("Main Head 2", Moving Head Light, 7)
RegisterDevice("Stage LEDs", RGB LED Strip, 13)
RegisterDevice("Fog", Fog Machine, 16)
RegisterDevice("Pyro", Pyrotechnics, 20)

// Create groups
CreateDeviceGroup("Moving Heads", ["Main Head 1", "Main Head 2"])
CreateDeviceGroup("Stage Effects", ["Fog", "Pyro"])

// Save presets
SavePreset("Opening", opening_config)
SavePreset("Performance", performance_config)
SavePreset("Finale", finale_config)

// When curtain opens
LoadPreset("Opening")
SetGroupParameter("Moving Heads", "intensity", 100)

// During performance
Timeline plays with dynamic lighting changes

// Grand finale
TriggerPyrotechnics("Pyro", Stage Location, "explosion", 1.0)
ActivateFogMachine("Fog", 0.8, 5.0)
SetGroupParameter("Stage LEDs", "animation", "strobe")
```

---

**Your Custom Devices Controller is ready for production use! 🚀✨**

Enjoy creating amazing interactive experiences with your UE5 project!
