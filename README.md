# UE5 RenderStream Remote Control Panel

🎮 **Professional remote control system for Unreal Engine 5.7 projects via RenderStream** - Control your scenes from any device (iPad, tablet, phone, or desktop)

## 🎯 Features

✅ **Real-time Control** - WebSocket-based two-way communication with zero lag
✅ **UE5.7 Native UI** - Matches Unreal Engine's dark theme and visual design
✅ **Multi-Project Support** - Control multiple UE5.7 instances simultaneously
✅ **Responsive Design** - Works perfectly on iPad, tablets, phones, and desktops
✅ **Disguise d3 Compatible** - Ready to integrate with RenderStream
✅ **No UE Rebuilds** - External server works with any UE5.7 project

## 🎛️ Control Features

### Lighting Control
- ☀️ Sun rotation adjustment (-360° to 360°)
- 💡 Sun intensity control (0.0 to 2.0)
- ⏰ Day/time cycle (0-24 hours)
- 📍 Sun position (X, Y, Z coordinates)

### Scene Management
- 🎬 Scene selector (switch between scenes)
- 🎨 Material color control (multiple materials)
- ⚡ Quick actions (Sunrise, Sunset, Noon, Reset)

### Advanced Features
- 📊 Real-time state synchronization
- 🔄 Automatic reconnection
- 📱 Touch-optimized controls
- 🌙 UE5 dark theme UI

## 📋 Requirements

- **Node.js** 16+ ([Download](https://nodejs.org/))
- **Unreal Engine 5.7** with RenderStream plugin enabled
- **Modern Browser** (Chrome, Firefox, Safari, Edge)
- **Network connectivity** between control devices and UE5 machine

## 🚀 Quick Start

### 1. Installation

```bash
# Clone the repository
git clone https://github.com/Andres-Felix/ue5-renderstream-remote-control.git
cd ue5-renderstream-remote-control

# Install dependencies
npm install
```

### 2. Configuration

```bash
# Copy environment template
cp .env.example .env

# Edit .env with your settings
nano .env
```

**.env Configuration:**
```env
PORT=3000
NODE_ENV=development
UE5_HOST=localhost          # Your UE5 machine IP or hostname
UE5_PORT=8080              # RenderStream API port
CORS_ORIGIN=*              # Allow all origins (restrict in production)
```

### 3. Configure Projects

Edit `config/projects.json` to add your UE5 projects:

```json
{
  "project-1": {
    "name": "Main Studio",
    "host": "192.168.1.100",
    "port": 8080,
    "active": true
  },
  "project-2": {
    "name": "Outdoor Render",
    "host": "render-server.local",
    "port": 8081,
    "active": true
  }
}
```

### 4. Start the Server

```bash
# Production
npm start

# Development (with auto-reload)
npm run dev
```

Server will be available at: **http://localhost:3000**

### 5. Access from iPad/Tablet/Phone

**On your control device:**
```
http://<server-ip>:3000
```

Replace `<server-ip>` with your server's IP address on the network.

## 🎮 Usage

### Basic Workflow

1. **Open the web interface** on your iPad/tablet/phone
2. **Select a project** from the sidebar
3. **Adjust controls** in real-time:
   - Use sliders for continuous adjustments
   - Use color pickers for material colors
   - Click quick action buttons for presets
4. **See changes instantly** on your UE5 project

### Control Types

| Control | Range | Use Case |
|---------|-------|----------|
| Sun Rotation | -360° to 360° | Change light direction |
| Sun Intensity | 0.0 to 2.0 | Adjust light brightness |
| Day Time | 0-24 hours | Animate time of day |
| Sun Position | XYZ coords | Move light in 3D space |
| Material Colors | RGB Hex | Change object colors |

## 🔌 UE5 Integration

### Receiving Commands in UE5

Your UE5 project needs an HTTP API endpoint to receive commands. Create a Blueprint or C++ function:

#### Blueprint Example:
```
1. Create a new Blueprint actor "RemoteControlReceiver"
2. Add an event dispatcher for each command type
3. Listen for HTTP POST requests on port 8080
4. Parse incoming JSON commands
5. Execute corresponding level functions
```

#### Command Format (JSON):
```json
{
  "command": "sun-rotation",
  "value": 45
}
```

### Example Commands

```json
// Rotate sun
{"command": "sun-rotation", "value": 90}

// Set sun intensity
{"command": "sun-intensity", "value": 1.5}

// Set time of day
{"command": "day-time", "value": 14.5}

// Change scene
{"command": "scene-change", "value": "outdoor"}

// Set material color
{"command": "mesh-color", "value": {"meshName": "mat_1", "color": "#FF0000"}}

// Update sun position
{"command": "sun-position", "value": {"x": 100, "y": 50, "z": 500}}
```

## 📡 Network Setup

### Local Network (LAN)

```
┌─────────────────┐
│  iPad/Tablet    │
│  :3000          │
└────────┬────────┘
         │ WebSocket (ws://)
         │ WiFi LAN
         │
┌────────▼────────┐         ┌──────────────────┐
│ Node.js Server  │────────→│  Unreal Engine   │
│ localhost:3000  │  HTTP   │  localhost:8080  │
└─────────────────┘         └──────────────────┘
```

### Remote Network

- Use a **VPN** or **SSH tunnel** for secure remote access
- Forward port 3000 through your network router
- Use **ngrok** for quick public access: `ngrok http 3000`

## 🐛 Troubleshooting

### Connection Issues

**Problem**: "Disconnected from server"
- Check server is running: `npm start`
- Verify firewall allows port 3000
- Check network connectivity between devices
- Ensure correct IP address in browser URL

### Commands Not Working

**Problem**: Sliders move but UE5 doesn't update
- Verify UE5 HTTP API is running and listening
- Check `config/projects.json` has correct host/port
- Verify RenderStream plugin is enabled in UE5
- Check UE5 project has command receiver implemented

### Performance Issues

**Problem**: Lag or slow response
- Check network latency: `ping <ue5-machine>`
- Reduce slider sensitivity (use larger step values)
- Check CPU/GPU load on both machines
- Use wired connection for better stability

## 📁 Project Structure

```
.
├── server.js                 # Main Node.js server
├── package.json              # Dependencies
├── .env.example             # Environment template
├── config/
│   └── projects.json        # Project configurations
├── public/
│   ├── index.html           # Main UI
│   ├── css/
│   │   └── style.css        # UE5-style dark theme
│   └── js/
│       └── app.js           # Frontend logic
└── README.md                # This file
```

## 🎨 Customization

### Adding New Controls

1. **Add HTML element** in `public/index.html`
2. **Add CSS styling** in `public/css/style.css`
3. **Add event listener** in `public/js/app.js`
4. **Send command** via `sendCommand(command, value)`
5. **Handle in UE5** with corresponding command receiver

### Theming

Edit CSS variables in `style.css` `:root` selector:

```css
:root {
  --primary-bg: #1e1e1e;          /* Main background */
  --accent-color: #0e639c;        /* Button color */
  --accent-light: #007acc;        /* Highlight color */
  --text-primary: #cccccc;        /* Main text */
}
```

## 🔐 Security Considerations

- ⚠️ **Development Mode**: Currently allows CORS from all origins
- 🔒 **Production**: Restrict CORS and add authentication
- 🔑 Add API key validation for sensitive commands
- 🛡️ Implement rate limiting to prevent abuse
- 🚨 Use HTTPS/WSS in production

## 📦 Deployment

### Docker (Recommended)

```dockerfile
FROM node:18-alpine
WORKDIR /app
COPY package*.json ./
RUN npm ci --only=production
COPY . .
EXPOSE 3000
CMD ["npm", "start"]
```

### Docker Compose

```yaml
version: '3.8'
services:
  remote-control:
    build: .
    ports:
      - "3000:3000"
    environment:
      - UE5_HOST=ue5-machine.local
      - UE5_PORT=8080
```

### Cloud Deployment

- **Heroku**: Add `Procfile`: `web: npm start`
- **AWS**: Use EC2 with Node.js AMI
- **DigitalOcean**: Simple Node.js one-click deployment
- **Render**: Connect GitHub repository directly

## 📚 API Reference

### REST Endpoints

```
GET  /api/projects
GET  /api/projects/:projectId/state
POST /api/projects/:projectId/command
GET  /health
```

### WebSocket Events

**Client → Server:**
- `select-project` - Select active project
- `control-command` - Send control command

**Server → Client:**
- `initial-state` - Send current state on connect
- `project-selected` - Confirm project selection
- `state-update` - Broadcast state changes
- `command-ack` - Acknowledge command receipt
- `error` - Send error messages

## 🤝 Contributing

Contributions welcome! Please:

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Commit your changes: `git commit -m 'Add amazing feature'`
4. Push to branch: `git push origin feature/amazing-feature`
5. Open a Pull Request

## 📄 License

MIT License - Feel free to use in commercial projects

## 💬 Support

- 📖 [Full Documentation](./docs)
- 🐛 [Report Issues](https://github.com/Andres-Felix/ue5-renderstream-remote-control/issues)
- 💡 [Discussions](https://github.com/Andres-Felix/ue5-renderstream-remote-control/discussions)

## 🙏 Acknowledgments

- **Unreal Engine 5.7** - Amazing real-time rendering
- **Disguise d3** - Professional media server
- **RenderStream** - Network rendering protocol
- **Socket.io** - Real-time WebSocket library
- **Express.js** - Web framework

---

**Made with ❤️ by Andres-Felix**

⭐ If you find this helpful, please star the repository!
