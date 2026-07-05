# Custom Devices Controller - Implementation Quickstart

## 🎯 Complete Integration Roadmap

This guide takes you from zero to a fully functional custom devices system in UE5.

---

## 📋 Phase 1: Project Setup (30 minutes)

### ✅ Checklist

- [ ] Copy C++ files to project
- [ ] Update `.Build.cs`
- [ ] Compile project in Visual Studio
- [ ] Verify no compilation errors

### Step 1.1: Copy C++ Files

```
YourProject/
└── Source/
    └── YourProject/
        ├── CustomDeviceController.h      ← Copy here
        ├── CustomDeviceController.cpp    ← Copy here
        ├── RemoteControlReceiver.h
        ├── RemoteControlReceiver.cpp
        └── YourProject.Build.cs
```

### Step 1.2: Update Build Configuration

Edit: `Source/YourProject/YourProject.Build.cs`

```csharp
public class YourProject : ModuleRules
{
    public YourProject(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HTTP",                    // ← Add
            "Json",                    // ← Add
            "JsonUtilities",           // ← Add
            "HttpServer"               // ← Add
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            // Add any private dependencies here
        });
    }
}
```

### Step 1.3: Compile Project

**Visual Studio:**
```
1. Open YourProject.sln
2. Build → Clean Solution
3. Build → Rebuild Solution
4. Wait for compilation (2-5 minutes)
```

**UE5 Editor:**
```
1. Tools → Compile
2. Wait for compilation
3. Check Output Log for errors
```

### Step 1.4: Verify Compilation

```
Expected Output:
✓ CustomDeviceController compiled successfully
✓ No errors or warnings
✓ Ready to create blueprints
```

---

## 🎮 Phase 2: Blueprint Setup (20 minutes)

### ✅ Checklist

- [ ] Create `BP_CustomDevices` blueprint
- [ ] Configure device controller settings
- [ ] Add to level
- [ ] Set up Level Blueprint

### Step 2.1: Create Main Blueprint

**In UE5 Editor:**

```
1. Content Browser → New → Blueprint Class
2. Search: "CustomDeviceController"
3. Select it
4. Name: "BP_CustomDevices"
5. Create
```

### Step 2.2: Configure Settings

**In BP_CustomDevices Details:**

```
Search for "Custom"
├─ Enable DMX Output: True
├─ Simulate Devices: True (for testing)
├─ Enable Logging: True (for debugging)
└─ DMX Universe: 1

Search for "HTTP"
├─ HTTP Server Port: 8080
└─ Max Client Rate: 100000
```

### Step 2.3: Add to Level

```
1. Open your level (or create new one)
2. Content Browser → Find BP_CustomDevices
3. Drag into viewport (anywhere is fine)
4. Save level (Ctrl + S)
```

### Step 2.4: Create Level Blueprint Reference

**In Level Blueprint (Ctrl + K):**

```blueprint
Variables:
├─ Name: "CustomDevices"
├─ Type: BP_CustomDevices Object Reference
└─ Instance Editable: True

Event BeginPlay:
├─ Get CustomDevices variable
├─ Connect to rest of setup
```

---

## ☀️ Phase 3: Register Devices (30 minutes)

### ✅ Checklist

- [ ] Register Moving Head lights
- [ ] Register RGB LED strips
- [ ] Register Fog machines
- [ ] Register Pyrotechnics
- [ ] Test device list in web panel

### Step 3.1: Basic Device Registration

**In Level Blueprint - Event BeginPlay:**

```blueprint
// Get the device controller
Get BP_CustomDevices → CustomDevices

// Register Moving Head
Register Device
├─ In Custom Devices: Get CustomDevices
├─ Device Name: "Stage Head 1"
├─ Device Type: Moving Head Light
├─ DMX Channel: 1

// Register RGB LED
Register Device
├─ In Custom Devices: Get CustomDevices
├─ Device Name: "Stage LEDs"
├─ Device Type: RGB LED Strip
├─ DMX Channel: 10

// Register Fog Machine
Register Device
├─ In Custom Devices: Get CustomDevices
├─ Device Name: "Fog Machine"
├─ Device Type: Fog Machine
├─ DMX Channel: 20

// Register Pyrotechnics
Register Device
├─ In Custom Devices: Get CustomDevices
├─ Device Name: "Pyrotechnics"
├─ Device Type: Pyrotechnics
├─ DMX Channel: 30
```

### Step 3.2: Create Setup Function

**Better Organization - Create Custom Event:**

```blueprint
Custom Event: SetupDevices

// Moving Heads (Channels 1-6)
Register Device("Head 1", MovingHead, 1)
Register Device("Head 2", MovingHead, 7)

// RGB LEDs (Channels 13-15)
Register Device("LED Strip 1", RGBLed, 13)

// Effects (Channels 20+)
Register Device("Fog", Fog, 20)
Register Device("Pyro", Pyrotechnics, 30)

Event BeginPlay:
├─ Call SetupDevices
└─ Print String: "Devices initialized"
```

### Step 3.3: Test Registration

```
1. Play level (Play button)
2. Open web panel: http://localhost:3000/devices.html
3. Check sidebar - should see all registered devices
4. Select each device - controls should appear
5. Stop play
```

---

## 🕯️ Phase 4: Moving Head Control (45 minutes)

### ✅ Checklist

- [ ] Create moving head actor
- [ ] Setup rotation blueprint
- [ ] Connect event dispatcher
- [ ] Test pan/tilt control
- [ ] Test color control

### Step 4.1: Create Moving Head Actor

**Blueprint: BP_MovingHead**

```blueprint
Components:
├─ Root: Scene Component
├─ Light Holder: Scene Component
│   └─ Directional Light
└─ Mesh: Static Mesh (optional - for visualization)

Variables:
├─ CurrentPan: Float (0-540)
├─ CurrentTilt: Float (0-270)
├─ CurrentIntensity: Float (0-100)
└─ CurrentColor: Linear Color
```

### Step 4.2: Implement Pan/Tilt Control

**In BP_MovingHead Event Graph:**

```blueprint
Custom Event: UpdatePanTilt
Input:
├─ Pan: Float (0-540)
├─ Tilt: Float (0-270)
└─ Duration: Float = 0.5

Implementation:
├─ Store CurrentPan = Pan
├─ Store CurrentTilt = Tilt
├─ Get Light Holder component
├─ Create Rotation from Euler
│   ├─ Pitch: Tilt value
│   ├─ Yaw: Pan value
│   └─ Roll: 0
├─ Set Actor Rotation
│   ├─ New Rotation: Created rotation
│   ├─ Teleport: False
│   └─ Sweep: True
└─ Print String: "Pan:Tilt Updated"
```

### Step 4.3: Connect Event Dispatcher

**In Level Blueprint:**

```blueprint
Event BeginPlay:
├─ Get CustomDevices
├─ Bind to Event
│   ├─ Event: On Moving Head Updated
│   ├─ Target: CustomDevices
│   └─ Create Event
│
Custom Event: OnMovingHeadUpdated
Input: MovingHeadState
├─ Get BP_MovingHead (by tag or reference)
├─ Call UpdatePanTilt
│   ├─ Pan: State.PanAngle
│   ├─ Tilt: State.TiltAngle
│   └─ Duration: 0.5
└─ Print String: "Moving Head Updated"
```

### Step 4.4: Add Color Control

**In BP_MovingHead:**

```blueprint
Custom Event: UpdateColor
Input:
├─ Color: Linear Color
└─ Duration: Float = 0.5

Implementation:
├─ Store CurrentColor = Color
├─ Get Directional Light component
├─ Set Light Color: Color
├─ Optional: Animate with Timeline
│   ├─ Lerp current color to new color
│   ├─ Over Duration seconds
│   └─ Update light each frame
```

### Step 4.5: Test Control

```
1. Place BP_MovingHead in level
2. Play
3. Open web panel: /devices.html
4. Select "Stage Head 1"
5. Drag pan/tilt sliders
6. Watch light rotate in viewport
7. Change color - light should update
8. Test XY pad for direct positioning
```

---

## 🌈 Phase 5: RGB LED Setup (40 minutes)

### ✅ Checklist

- [ ] Create RGB LED material
- [ ] Create LED strip actor
- [ ] Setup color animation
- [ ] Connect event dispatcher
- [ ] Test all animation modes

### Step 5.1: Create LED Material

**Material: M_LEDStrip**

```blueprint
Material Graph:
├─ Base Color: Parameter "LED_Color"
├─ Emissive Color: LED_Color × Brightness Parameter
├─ Metallic: 0.0
├─ Roughness: 0.5
└─ Output:
    ├─ Base Color: ← LED_Color
    └─ Emissive: ← LED_Color × Brightness
```

### Step 5.2: Create LED Actor

**Blueprint: BP_RGBLEDStrip**

```blueprint
Components:
├─ Root: Scene Component
├─ Mesh: Static Mesh (plane or ribbon)
│   └─ Material: M_LEDStrip
└─ Light: Point Light (for glow)

Variables:
├─ CurrentColor: Linear Color
├─ CurrentBrightness: Float
├─ AnimationMode: String
├─ AnimationSpeed: Float
├─ AnimationTimeline: Reference to Timeline
└─ MaterialInstance: Dynamic Material Instance
```

### Step 5.3: Setup Animation Timeline

**Create Timeline: "LEDAnimation"**

```blueprint
Timeline: LEDAnimation
├─ Length: 5.0 seconds (loops)
├─ Looping: True
├─ Tracks:
│   └─ ColorTrack (Linear Color)
│       ├─ Key @ 0.0s: Red (1, 0, 0, 1)
│       ├─ Key @ 1.0s: Green (0, 1, 0, 1)
│       ├─ Key @ 2.0s: Blue (0, 0, 1, 1)
│       ├─ Key @ 3.0s: Yellow (1, 1, 0, 1)
│       ├─ Key @ 4.0s: Magenta (1, 0, 1, 1)
│       └─ Key @ 5.0s: Red (1, 0, 0, 1)
```

### Step 5.4: Implement Color Control

**In BP_RGBLEDStrip:**

```blueprint
Event BeginPlay:
├─ Get Mesh component
├─ Create Dynamic Material Instance
├─ Store in MaterialInstance variable

Custom Event: UpdateColor
Input:
├─ Color: Linear Color
├─ Brightness: Float
├─ Mode: String

Implementation:
├─ Store CurrentColor = Color
├─ Store CurrentBrightness = Brightness
├─ Store AnimationMode = Mode
├─
├─ Set Material Scalar Parameter
│   ├─ Parameter: "Brightness"
│   └─ Value: Brightness
├─
├─ Set Material Vector Parameter
│   ├─ Parameter: "LED_Color"
│   └─ Value: Color
├─
├─ Switch on Mode:
│   ├─ Case "static": Stop animation
│   ├─ Case "pulse": Play pulse timeline
│   ├─ Case "rainbow": Play rainbow timeline
│   ├─ Case "chase": Play chase animation
│   └─ Case "strobe": Play strobe effect
```

### Step 5.5: Animation Modes Implementation

**Static Mode:**
```blueprint
Stop Timeline: LEDAnimation
Set Material Color: CurrentColor
```

**Pulse Mode:**
```blueprint
Create Pulse Timeline (0-1 normalized brightness)
On Timeline Update:
├─ Lerp(0, CurrentBrightness, TimelineValue)
├─ Set Material Brightness
```

**Rainbow Mode:**
```blueprint
Cycle through color spectrum
├─ Red → Orange → Yellow → Green → Blue → Purple → Red
├─ Duration based on AnimationSpeed
```

**Chase Mode:**
```blueprint
For loop moving along LED strip
├─ Current position cycles 0 to 1
├─ Only nearby LEDs are bright
```

### Step 5.6: Test LED Control

```
1. Place BP_RGBLEDStrip in level
2. Play
3. Select "Stage LEDs" in web panel
4. Change color - LED updates immediately
5. Test animation modes dropdown
6. Adjust brightness slider
7. Change animation speed
```

---

## ☁️ Phase 6: Fog Machine (25 minutes)

### ✅ Checklist

- [ ] Create fog particle effect
- [ ] Create fog machine actor
- [ ] Setup density control
- [ ] Connect event dispatcher
- [ ] Test activation

### Step 6.1: Create Fog Particle System

**Particle System: PS_Fog**

```blueprint
Emitter: Fog
├─ Spawn Rate: 1000 (adjustable)
├─ Particle Lifetime: 5-10 seconds
├─ Initial Size: 10-50
├─ Velocity:
│   ├─ Initial: (0, 0, 50) - upward
│   └─ Damping: 0.5
├─ Color: White with transparency
└─ Material: Translucent fog effect
```

### Step 6.2: Create Fog Machine Actor

**Blueprint: BP_FogMachine**

```blueprint
Components:
├─ Root: Scene Component
├─ Particle System: Niagara/Cascade (PS_Fog)
└─ Speaker: Audio Component (optional - sound effect)

Variables:
├─ CurrentDensity: Float
├─ IsActive: Boolean
├─ MaxSpawnRate: Float = 1000
└─ CurrentDuration: Float
```

### Step 6.3: Implement Fog Control

**In BP_FogMachine:**

```blueprint
Custom Event: ActivateFog
Input:
├─ Density: Float (0-1)
└─ Duration: Float (seconds)

Implementation:
├─ Store IsActive = True
├─ Store CurrentDensity = Density
├─ Store CurrentDuration = Duration
├─
├─ Get Particle System component
├─ Set Spawn Rate: Density × MaxSpawnRate
├─ Activate Particle System
├─
├─ Delay: Duration seconds
├─ Deactivate Particle System
└─ Store IsActive = False

Custom Event: DeactivateFog
├─ Store IsActive = False
├─ Get Particle System component
├─ Deactivate Particle System
```

### Step 6.4: Connect Event Dispatcher

**In Level Blueprint:**

```blueprint
Bind to CustomDevices:
├─ On Fog Machine Triggered
├─
Custom Event: OnFogTriggered
Input: State
├─ Get BP_FogMachine (by reference)
├─ Call ActivateFog
│   ├─ Density: State.Density
│   └─ Duration: State.Duration
```

### Step 6.5: Test Fog Control

```
1. Place BP_FogMachine in level
2. Play
3. Select "Fog Machine" in web panel
4. Click Activate button
5. Watch fog particles appear
6. Adjust density slider
7. Set duration and trigger again
```

---

## 💥 Phase 7: Pyrotechnics (35 minutes)

### ✅ Checklist

- [ ] Create pyro effect particles
- [ ] Create pyro actor with safety
- [ ] Setup arm/disarm logic
- [ ] Setup trigger with safety checks
- [ ] Test full safety sequence

### Step 7.1: Create Pyro Effects

**Particle Systems:**

```
PS_Explosion:
├─ Large burst of particles
├─ Camera shake on spawn
└─ Sound effect

PS_Fire:
├─ Flame effect going upward
├─ Orange/yellow colors
└─ Light flicker

PS_Smoke:
├─ Smoke cloud
├─ Gray/white particles
└─ Slow rise
```

### Step 7.2: Create Pyrotechnics Actor

**Blueprint: BP_Pyrotechnics**

```blueprint
Components:
├─ Root: Scene Component
├─ Audio: Audio Component
└─ Trigger Volume: Box Component (optional)

Variables:
├─ IsArmed: Boolean = False
├─ IsSafetyLocked: Boolean = True
├─ AllowedEffects: Array of Strings
└─ CurrentEffect: String
```

### Step 7.3: Implement Safety System

**In BP_Pyrotechnics:**

```blueprint
Custom Event: ArmPyrotechnics
├─ Check if IsArmed = True already
├─ If yes, return "Already armed"
├─ If no:
│   ├─ Store IsArmed = True
│   ├─ Store IsSafetyLocked = False
│   ├─ Play alert sound (optional)
│   └─ Print String: "PYROTECHNICS ARMED - Be careful!"

Custom Event: DisarmPyrotechnics
├─ Store IsArmed = False
├─ Store IsSafetyLocked = True
├─ Print String: "PYROTECHNICS DISARMED - Safe"

Custom Event: TriggerPyrotechnics
Input:
├─ EffectType: String
├─ Location: Vector
└─ Intensity: Float (0-1)

Implementation:
├─ Check if IsArmed = True
│   └─ If False: Print "Not armed!" and return
├─
├─ Check if IsSafetyLocked = True
│   └─ If True: Print "Safety locked!" and return
├─
├─ If all checks pass:
│   ├─ Spawn particle system based on EffectType
│   ├─ At Location with Intensity scale
│   ├─ Play explosion sound
│   ├─ Apply camera shake to nearby players
│   └─ Optional: Apply physical force to nearby objects
```

### Step 7.4: Connect Event Dispatcher

**In Level Blueprint:**

```blueprint
Bind to CustomDevices:
├─ On Pyrotechnics Triggered
├─
Custom Event: OnPyroTriggered
Input: State
├─ Check if State.bArmed = True
├─ Check if State.bSafetyLocked = False
├─
├─ Get BP_Pyrotechnics (by reference)
├─ Call TriggerPyrotechnics
│   ├─ EffectType: State.EffectType
│   ├─ Location: State.Location
│   └─ Intensity: State.Intensity
```

### Step 7.5: Test Safety Sequence

```
1. Place BP_Pyrotechnics in level
2. Play
3. Select "Pyrotechnics" in web panel
4. Try clicking TRIGGER - should be blocked
5. Click ARM button - now armed
6. Try clicking TRIGGER - should work!
7. Effect should appear at location
8. Click DISARM - resets safety
```

---

## 👥 Phase 8: Device Groups (20 minutes)

### ✅ Checklist

- [ ] Create device groups
- [ ] Setup group control blueprint
- [ ] Test controlling multiple devices
- [ ] Save as preset

### Step 8.1: Create Groups

**In Level Blueprint - After Device Registration:**

```blueprint
Custom Event: SetupGroups

Create Device Group
├─ Group Name: "Moving Heads"
├─ Devices:
│   ├─ "Stage Head 1"
│   └─ "Stage Head 2"

Create Device Group
├─ Group Name: "Stage Effects"
├─ Devices:
│   ├─ "Fog Machine"
│   ├─ "Pyrotechnics"
│   └─ "Stage LEDs"

Create Device Group
├─ Group Name: "All Lights"
├─ Devices:
│   ├─ "Stage Head 1"
│   ├─ "Stage Head 2"
│   └─ "Stage LEDs"
```

### Step 8.2: Implement Group Control

**In Level Blueprint - Custom Events:**

```blueprint
Event: ControlMovingHeadGroup
├─ Set Group Parameter
│   ├─ Group: "Moving Heads"
│   ├─ Parameter: "intensity"
│   └─ Value: 100
└─ Result: Both heads go to full intensity

Event: TriggerAllEffects
├─ Trigger Group Effect
│   ├─ Group: "Stage Effects"
│   └─ Effect: "finale"
└─ Result: Fog activates + Pyro triggers
```

### Step 8.3: Test Groups

```
1. Play
2. In web panel, use group controls
3. Change one parameter
4. See all devices in group update
5. Verify synchronization
```

---

## 💾 Phase 9: Presets (25 minutes)

### ✅ Checklist

- [ ] Create preset system
- [ ] Save common scenes
- [ ] Load presets
- [ ] Test scene transitions

### Step 9.1: Define Presets

**Create JSON Structures - In Level Blueprint:**

```blueprint
// Preset: Opening Scene
Preset_Opening = {
  "moving_heads": [
    {"name": "Stage Head 1", "pan": 180, "tilt": 45, "intensity": 0},
    {"name": "Stage Head 2", "pan": 180, "tilt": 45, "intensity": 0}
  ],
  "rgb_leds": [
    {"name": "Stage LEDs", "color": "#000000", "brightness": 0}
  ],
  "fog": {"density": 0, "active": false},
  "timestamp": "2024-06-12T15:30:00Z"
}

// Preset: Performance
Preset_Performance = {
  "moving_heads": [
    {"name": "Stage Head 1", "pan": 0, "tilt": 60, "intensity": 100, "color": "#ffffff"},
    {"name": "Stage Head 2", "pan": 0, "tilt": 60, "intensity": 100, "color": "#ffffff"}
  ],
  "rgb_leds": [
    {"name": "Stage LEDs", "color": "#0088ff", "brightness": 80}
  ],
  "fog": {"density": 0.3, "active": true}
}

// Preset: Finale
Preset_Finale = {
  "moving_heads": [
    {"name": "Stage Head 1", "pan": 180, "tilt": 45, "intensity": 100, "strobe": 50},
    {"name": "Stage Head 2", "pan": 180, "tilt": 45, "intensity": 100, "strobe": 50}
  ],
  "rgb_leds": [
    {"name": "Stage LEDs", "color": "#ff00ff", "brightness": 100, "animation": "strobe"}
  ],
  "fog": {"density": 1.0, "active": true},
  "pyro": "arm"
}
```

### Step 9.2: Implement Save/Load

**In Level Blueprint:**

```blueprint
Custom Event: SaveScenePreset
Input: PresetName (String)
├─ Get all device states
├─ Collect into JSON structure
├─ Call CustomDevices.SavePreset
│   ├─ Preset Name: PresetName
│   └─ Preset Data: JSON structure

Custom Event: LoadScenePreset
Input: PresetName (String)
├─ Call CustomDevices.LoadPreset
│   └─ Preset Name: PresetName
├─
├─ For Each Device in Preset:
│   ├─ Set device to saved state
│   ├─ Animate transition (0.5-2 seconds)
│   └─ Update UI
```

### Step 9.3: Create Save Buttons

**In Level Blueprint - Setup Functions:**

```blueprint
Create Custom Events for Presets:
├─ Event: SaveAsOpening
│   └─ SaveScenePreset("Opening")
│
├─ Event: SaveAsPerformance
│   └─ SaveScenePreset("Performance")
│
├─ Event: SaveAsFinale
│   └─ SaveScenePreset("Finale")
│
├─ Event: LoadOpening
│   └─ LoadScenePreset("Opening")
│
├─ Event: LoadPerformance
│   └─ LoadScenePreset("Performance")
│
└─ Event: LoadFinale
    └─ LoadScenePreset("Finale")
```

### Step 9.4: Test Presets

```
1. Configure devices manually
2. Save as "Opening"
3. Change devices to different values
4. Load "Opening" - devices should revert
5. Create multiple presets
6. Test transitions between them
```

---

## 🌐 Phase 10: Web Panel Integration (15 minutes)

### ✅ Checklist

- [ ] Access web panel in browser
- [ ] See all registered devices
- [ ] Test each device control
- [ ] Test device selection
- [ ] Test notifications

### Step 10.1: Start Web Server

**Terminal:**
```bash
cd ue5-renderstream-remote-control
npm install
npm start
```

**Output:**
```
Server running at http://localhost:3000
Custom Devices available at http://localhost:3000/devices.html
```

### Step 10.2: Access Control Panel

**In Browser:**
```
Main Panel: http://localhost:3000
Custom Devices: http://localhost:3000/devices.html
```

### Step 10.3: Verify Connection

```
Web Panel:
✓ Status shows "Connected"
✓ All devices appear in sidebar
✓ Can click devices to select them
✓ Controls appear based on device type
```

### Step 10.4: Test Each Device Type

**Moving Head:**
- [ ] Pan slider moves light
- [ ] Tilt slider moves light
- [ ] Color picker changes light color
- [ ] XY pad works for position
- [ ] Intensity slider brightens light
- [ ] Zoom slider zooms light
- [ ] Strobe effect works

**RGB LED:**
- [ ] Color picker changes LED color
- [ ] Brightness slider works
- [ ] Animation dropdown changes mode
- [ ] Speed slider changes animation
- [ ] Presets (Warm/Cool/Party) work

**Fog Machine:**
- [ ] Activate button starts fog
- [ ] Density slider controls fog amount
- [ ] Duration setting works
- [ ] Deactivate stops fog

**Pyrotechnics:**
- [ ] Arm button enables controls
- [ ] Effect dropdown works
- [ ] Intensity slider adjusts effect
- [ ] Trigger button fires effect
- [ ] Disarm button resets

### Step 10.5: Test Notifications

```
Success:
✓ Green toast appears on control
✓ Message confirms action
✓ Auto-dismiss after 3 seconds

Error:
✓ Red toast on failed command
✓ Clear error message
✓ Doesn't break subsequent commands
```

---

## 🧪 Phase 11: Testing & Debugging (30 minutes)

### ✅ Checklist

- [ ] Enable logging in BP_CustomDevices
- [ ] Monitor Output Log
- [ ] Test all commands work
- [ ] Test error handling
- [ ] Verify performance

### Step 11.1: Enable Logging

**In BP_CustomDevices Details:**
```
Enable Logging: True
```

### Step 11.2: Monitor Output Log

**In UE5 Editor:**
```
Window → Output Log

Look for:
[CustomDevices] Device registered: 'Stage Head 1'
[CustomDevices] Moving Head 'Stage Head 1' Pan: 180.00°
[CustomDevices] RGB LED 'Stage LEDs' Color: RGB(0.00, 0.88, 1.00)
```

### Step 11.3: Test Error Handling

```
Try sending commands to:
✓ Non-existent device → Error logged
✓ Wrong device type → Error logged
✓ Invalid parameters → Clamped to valid range
✓ Rapid commands → No lag or stuttering
```

### Step 11.4: Performance Check

**In Editor:**
```
Stat Unit (Backtick key)
Check:
- FPS stays above 60
- Game thread under 16ms
- GPU under 16ms
- No performance spikes
```

---

## 🚀 Phase 12: Deployment (20 minutes)

### ✅ Checklist

- [ ] Package project
- [ ] Test on different device
- [ ] Share network access
- [ ] Document setup

### Step 12.1: Package Project

**In UE5 Editor:**
```
File → Package Project → Windows (64-bit)
├─ Create new folder
├─ Select folder
└─ Wait for packaging (5-15 minutes)
```

### Step 12.2: Run Packaged Version

```
Navigate to packaged folder:
YourProject\Binaries\Win64\
├─ Run: YourProject.exe
├─ Server starts automatically
└─ Open browser to localhost:3000
```

### Step 12.3: Network Access

**From other devices on network:**
```
Browser: http://<your-machine-ip>:3000/devices.html

Example: http://192.168.1.100:3000/devices.html
```

### Step 12.4: Documentation

**Create README for your team:**
```markdown
# Setup Instructions

1. Run YourProject.exe
2. Open http://localhost:3000/devices.html
3. Or from network: http://<IP>:3000/devices.html

## Devices:
- Stage Head 1 - Moving head light (Pan/Tilt)
- Stage Head 2 - Moving head light (Pan/Tilt)
- Stage LEDs - RGB LED strip
- Fog Machine - Fog effect
- Pyrotechnics - Pyro effects

## Safety:
- Pyrotechnics require ARM before use
- Safety lock prevents accidental trigger
- Always DISARM after use

## Presets:
- Opening: Initial setup
- Performance: Main scene lighting
- Finale: High energy with effects
```

---

## 📊 Complete Integration Checklist

### Phase 1: Project Setup
- [ ] C++ files copied
- [ ] Build.cs updated
- [ ] Project compiled
- [ ] No errors

### Phase 2: Blueprint Setup
- [ ] BP_CustomDevices created
- [ ] Settings configured
- [ ] Added to level
- [ ] Level Blueprint setup

### Phase 3: Device Registration
- [ ] All devices registered
- [ ] DMX channels assigned
- [ ] Web panel shows devices
- [ ] Device list verified

### Phase 4: Moving Head
- [ ] Actor created
- [ ] Pan/tilt working
- [ ] Color working
- [ ] Event dispatcher connected
- [ ] Web control working

### Phase 5: RGB LED
- [ ] Material created
- [ ] Actor created
- [ ] Color control working
- [ ] Animations working
- [ ] Web control working

### Phase 6: Fog Machine
- [ ] Particles created
- [ ] Actor created
- [ ] Activation working
- [ ] Density control working
- [ ] Web control working

### Phase 7: Pyrotechnics
- [ ] Effects created
- [ ] Safety system working
- [ ] Arm/disarm working
- [ ] Trigger safe
- [ ] Web control working

### Phase 8: Device Groups
- [ ] Groups created
- [ ] Group control working
- [ ] Multi-device sync working

### Phase 9: Presets
- [ ] Presets saved
- [ ] Presets load correctly
- [ ] Transitions smooth

### Phase 10: Web Panel
- [ ] Loads at /devices.html
- [ ] Shows all devices
- [ ] All controls responsive
- [ ] Notifications work

### Phase 11: Testing
- [ ] Logging enabled
- [ ] No errors in log
- [ ] Performance good (60+ FPS)
- [ ] Error handling works

### Phase 12: Deployment
- [ ] Project packaged
- [ ] Runs standalone
- [ ] Network access works
- [ ] Team docs created

---

## 🎉 Success! You're Done!

Congratulations! You now have a fully functional custom devices control system:

✅ **Complete Web Control Panel** - Modern UI for all devices
✅ **Professional Integration** - Seamlessly integrated with UE5
✅ **Safety Features** - Especially for pyrotechnics
✅ **Preset System** - Save/load configurations
✅ **Device Groups** - Control multiple at once
✅ **Network Control** - Access from anywhere
✅ **Performance Optimized** - 60+ FPS maintained
✅ **Production Ready** - Packaged and deployable

---

## 📞 Troubleshooting Reference

### Connection Issues
```
Problem: Web panel says "Disconnected"
Solution:
1. Verify server is running (npm start)
2. Check http://localhost:3000 loads
3. Restart both UE5 and web server
4. Check Windows Firewall allows port 3000
```

### Device Not Responding
```
Problem: Device appears in list but doesn't respond
Solution:
1. Enable logging in BP_CustomDevices
2. Check Output Log for errors
3. Verify device is registered in Level Blueprint
4. Check device name matches exactly
5. Verify event dispatcher is connected
```

### Performance Issues
```
Problem: Frame rate drops when using controls
Solution:
1. Check Stat Unit (backtick key)
2. Reduce number of simultaneous device updates
3. Disable logging if debugging complete
4. Check for expensive Blueprint operations
```

### Safety Lock Won't Release
```
Problem: Pyrotechnics safety locked after arm
Solution:
1. This is intentional - double security
2. Click DISARM button to release
3. Safety automatically resets after trigger
4. Check web console for confirmation
```

---

## 🎓 Next Learning Steps

After completing this guide:

1. **Custom Device Types** - Extend with your own devices
2. **Hardware Integration** - Connect to real DMX equipment
3. **Advanced Automation** - Create complex lighting sequences
4. **Mobile App** - Make native control app
5. **Cloud Control** - Control from internet via ngrok
6. **AI Integration** - Automate lighting based on game events

---

**You're now ready to create amazing interactive lighting experiences in UE5! 🎊**

Start with Phase 1 and work your way through each phase systematically. Each phase builds on the previous one.

Good luck and have fun! 🚀
