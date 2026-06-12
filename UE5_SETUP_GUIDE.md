# UE5 Blueprint & Animation Setup Guide

## 📋 Prerequisites

Before you start, ensure you have:
- UE5.3+ installed
- Visual Studio 2022 with C++ development tools
- Your project with C++ support enabled

---

## 🔧 Step 1: Add C++ to Your Project

### Option A: Create New C++ Project
1. **Unreal Engine Launcher** → **Create Project**
2. Choose **Games** → **Blank** (C++)
3. Set **Target Platform**: Windows/Linux
4. Create project
5. Copy the `RemoteControlReceiver.h` and `RemoteControlReceiver.cpp` files to:
   ```
   YourProject/Source/YourProject/
   ```

### Option B: Add to Existing Blueprint Project
1. **File** → **New C++ Class**
2. Choose **Actor** as parent class
3. Name it `RemoteControlReceiver`
4. Finish and open Visual Studio
5. Replace the generated files with the provided header and cpp files

---

## 📝 Step 2: Update Project Build Configuration

### Edit `YourProject.Build.cs`

Locate: `Source/YourProject/YourProject.Build.cs`

Replace the `PublicDependencyModuleNames` section:

```csharp
PublicDependencyModuleNames.AddRange(
    new string[] {
        "Core",
        "CoreUObject",
        "Engine",
        "InputCore",
        "HTTP",                    // Add this line
        "Json",                    // Add this line
        "JsonUtilities",           // Add this line
        "HttpServer"               // Add this line
    }
);
```

---

## 🏗️ Step 3: Compile & Build

### Visual Studio
1. Open `YourProject.sln` in Visual Studio
2. **Build** → **Rebuild Solution**
3. Wait for compilation to complete

### Unreal Editor
1. **Tools** → **Compile**
2. Wait for recompilation
3. If errors occur, check **Output Log** for details

---

## 🎮 Step 4: Create Blueprint Actor

### Step 4A: Create Blueprint
1. In **Content Browser**, navigate to any folder
2. **+ Add** → **Blueprint Class**
3. Search for **RemoteControlReceiver**
4. Select it and click **Create Blueprint**
5. Name it: `BP_RemoteControl`

### Step 4B: Configure Blueprint
1. Open `BP_RemoteControl`
2. In **Details Panel**, set:
   - **HTTP Server Port**: `8080` (or your preferred port)
   - **Enable Logging**: `True` (for debugging)
   - **Max Sun Intensity**: `2.0`

### Step 4C: Add to Level
1. Open your level (e.g., `Level_Main`)
2. Drag `BP_RemoteControl` into the viewport
3. Position it anywhere (it's logic-only)
4. **Save** the level

---

## 🔌 Step 5: Connect Blueprint Events

### Setup Event Dispatchers in Level Blueprint

1. Open **Level Blueprint** (keyboard: `Ctrl + K`)
2. Right-click → **Add Event for BP_RemoteControl** → **Event Begin Play**
3. From the `BP_RemoteControl` reference, drag out and search for:

#### **On Sun Rotation Changed**
```
BP_RemoteControl → On Sun Rotation Changed
├─ Print String: "Sun Rotation: {Rotation}"
└─ Set Directional Light Rotation (connect to your light actor)
```

#### **On Day Time Changed**
```
BP_RemoteControl → On Day Time Changed
├─ Convert Hours to Rotator (0-24 → 0-360°)
└─ Set Directional Light Rotation
```

#### **On Sun Intensity Changed**
```
BP_RemoteControl → On Sun Intensity Changed
├─ Set Directional Light Intensity
```

#### **On Sun Position Changed**
```
BP_RemoteControl → On Sun Position Changed
├─ Add Actor World Offset (to move light actor)
```

#### **On Material Color Changed**
```
BP_RemoteControl → On Material Color Changed
├─ Get Actor by Tag (e.g., "Materials")
├─ Cast to Static Mesh Actor
└─ Set Material Scalar Parameter
```

#### **On Quick Action Executed**
```
BP_RemoteControl → On Quick Action Executed
├─ Switch on String (ActionName)
├─ Case "sunrise": Set Day Time to 6
├─ Case "sunset": Set Day Time to 18
├─ Case "noon": Set Day Time to 12
└─ Case "reset": Reset all to defaults
```

---

## ☀️ Step 6: Setup Directional Light Control

### Create Blueprint Function for Light Control

1. Create new **Blueprint Function**: `UpdateSunLight`
2. Inputs:
   - `Rotation` (Float)
   - `Intensity` (Float)
3. Implementation:
   ```
   Get Actor with Tag "SunLight"
   ├─ Cast to Light
   ├─ Set Actor Rotation (from Rotation value)
   ├─ Set Light Intensity
   └─ Print "Light Updated"
   ```

### Connect to Remote Control

1. In Level Blueprint
2. **On Sun Rotation Changed** → Call `UpdateSunLight`
3. **On Sun Intensity Changed** → Call `UpdateSunLight`

---

## 🎨 Step 7: Setup Material Control

### Create Material Master

1. **Content Browser** → **+ New** → **Material**
2. Name: `M_ControlledObject`
3. Add parameters:
   - **Param_BaseColor** (Vector3)
   - **Param_Metallic** (Scalar)
   - **Param_Roughness** (Scalar)

### Setup Actor with Material

1. Create **Blueprint Actor**: `BP_ControlledObject`
2. Add **Static Mesh Component**
3. Set Material to `M_ControlledObject`
4. Add Tag: `"Controllable"`
5. Place in level

### Connect Color Control

1. In Level Blueprint
2. **On Material Color Changed**:
   ```
   Get All Actors with Interface "Controllable"
   ├─ For Each
   │   ├─ Get Static Mesh Component
   │   ├─ Create Dynamic Material Instance
   │   └─ Set Vector Parameter "Param_BaseColor"
   ```

---

## ⏱️ Step 8: Day/Time Cycle Animation

### Create Blueprint for Time Progression

1. Create new **Blueprint Actor**: `BP_DayNightCycle`
2. Add this logic:
   ```
   Tick Event (every frame):
   ├─ Get BP_RemoteControl
   ├─ Get CurrentDayTime
   ├─ Convert to Sun Angle (Hours × 15 = Degrees)
   ├─ Calculate Sky Light Color
   │   ├─ 0:00 (Night) → Dark Blue (#001a4d)
   │   ├─ 6:00 (Sunrise) → Orange (#ff8c00)
   │   ├─ 12:00 (Noon) → Bright Blue (#87ceeb)
   │   └─ 18:00 (Sunset) → Red (#ff4500)
   ├─ Lerp between colors
   └─ Update Sky Light
   ```

---

## 📡 Step 9: Network Configuration

### Enable Network Play

In **Project Settings**:

1. **Edit** → **Project Settings**
2. Search: **"net"**
3. Under **Network**:
   ```
   Max Client Rate: 100000
   Max Server Rate: 100000
   ```

4. Search: **"http"**
5. Under **HTTP**:
   ```
   Enable HTTP Server: True
   HTTP Server Port: 8080
   ```

---

## 🧪 Step 10: Testing Setup

### Test in Editor

1. **Play** in viewport
2. Open browser: `http://localhost:8080`
3. You should see the Remote Control Panel
4. Try moving sliders and watch your level update in real-time

### Test with Standalone Game

1. **File** → **Package Project** → **Windows (64-bit)**
2. Run the `.exe`
3. Open `http://<your-machine-ip>:8080` from another device
4. Control the game from anywhere on your network!

---

## 🐛 Troubleshooting

### Problem: "HTTP Server failed to start"

**Solution:**
- Check if port 8080 is already in use
- Run as Administrator
- Change port number in BP_RemoteControl Details

### Problem: "Commands not received"

**Solution:**
- Enable **Enable Logging** in BP_RemoteControl
- Check **Output Log** for messages
- Verify URL format in web panel
- Check Windows Firewall settings

### Problem: "Blueprint events not firing"

**Solution:**
- Verify BP_RemoteControl is placed in level
- Check Level Blueprint for proper event connections
- Ensure actors have correct Tags
- Check console for binding errors

### Problem: "Compilation errors"

**Solution:**
- Rebuild Visual Studio solution
- Delete `Binaries`, `Intermediate`, `.vs` folders
- Right-click `.uproject` → **Generate Visual Studio project files**
- Reopen in Visual Studio and rebuild

---

## 📋 Quick Setup Checklist

- [ ] Copy `.h` and `.cpp` files to `Source/YourProject/`
- [ ] Add HTTP, Json, JsonUtilities to `.Build.cs`
- [ ] Compile project
- [ ] Create `BP_RemoteControl` Blueprint
- [ ] Add to level
- [ ] Create `BP_RemoteControl` in Level Blueprint
- [ ] Connect Event Dispatchers
- [ ] Create `BP_DayNightCycle` for time control
- [ ] Setup Directional Light
- [ ] Setup controllable materials
- [ ] Configure network settings
- [ ] Test in editor
- [ ] Package and test standalone

---

## 🚀 Next Steps

1. **Advanced Animations**: Add smooth transitions to time changes
2. **Weather System**: Integrate rain/clouds based on time
3. **Camera Control**: Add camera positioning via remote
4. **Actor Spawning**: Dynamically spawn/despawn objects
5. **Save/Load States**: Save control presets

---

## 📖 API Reference

### Blueprint Callable Functions

```cpp
// Set individual controls
SetSunRotation(Rotation: float)
SetSunIntensity(Intensity: float)
SetDayTime(Hours: float)  // 0-24
SetSunPosition(Position: FVector)
ChangeScene(SceneName: string)
SetMaterialColor(MaterialName: string, Color: LinearColor)
ExecuteQuickAction(ActionName: string)

// Get state
GetCurrentState() -> string (JSON)
```

### Available Events

- `OnSunRotationChanged(Rotation, bSuccess)`
- `OnSunIntensityChanged(Intensity, bSuccess)`
- `OnDayTimeChanged(Hours, bSuccess)`
- `OnSunPositionChanged(Position, bSuccess)`
- `OnSceneChanged(SceneName, bSuccess)`
- `OnMaterialColorChanged(MaterialName, bSuccess)`
- `OnQuickActionExecuted(ActionName)`

---

## 💾 File Locations

```
YourProject/
├── Source/
│   └── YourProject/
│       ├── RemoteControlReceiver.h      ← C++ Header
│       ├── RemoteControlReceiver.cpp    ← C++ Implementation
│       └── YourProject.Build.cs         ← Build configuration
├── Content/
│   └── Blueprints/
│       ├── BP_RemoteControl.uasset      ← Main actor
│       ├── BP_DayNightCycle.uasset      ← Time system
│       └── BP_ControlledObject.uasset   ← Controllable objects
└── YourProject.uproject
```

---

## 📞 Support

For issues or questions:
1. Check **Output Log** in UE5 editor
2. Review troubleshooting section
3. Check GitHub issues
4. Review HTTP server logs

---

**Happy remote controlling! 🎮✨**
