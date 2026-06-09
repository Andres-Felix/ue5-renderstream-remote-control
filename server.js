const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const cors = require('cors');
const path = require('path');
const axios = require('axios');
require('dotenv').config();

const app = express();
const server = http.createServer(app);
const io = socketIo(server, {
  cors: {
    origin: process.env.CORS_ORIGIN || '*',
    methods: ['GET', 'POST']
  }
});

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

const PORT = process.env.PORT || 3000;

// Store active projects and their state
const projectStates = {};

// Load projects configuration
let projects = {};
try {
  projects = require('./config/projects.json');
} catch (e) {
  console.log('Projects config not found, using defaults');
  projects = {
    'default': {
      name: 'Default Project',
      host: process.env.UE5_HOST || 'localhost',
      port: process.env.UE5_PORT || 8080,
      active: true
    }
  };
}

// Initialize project states
Object.keys(projects).forEach(projectId => {
  projectStates[projectId] = {
    sunRotation: 0,
    sunIntensity: 1.0,
    dayTime: 12,
    currentScene: 'default',
    meshColors: {},
    sunPosition: { x: 0, y: 0, z: 0 },
    connected: false
  };
});

// REST API Endpoints
app.get('/api/projects', (req, res) => {
  res.json({
    projects: Object.keys(projects).map(id => ({
      id,
      ...projects[id],
      state: projectStates[id]
    }))
  });
});

app.get('/api/projects/:projectId/state', (req, res) => {
  const { projectId } = req.params;
  if (!projectStates[projectId]) {
    return res.status(404).json({ error: 'Project not found' });
  }
  res.json(projectStates[projectId]);
});

app.post('/api/projects/:projectId/command', async (req, res) => {
  const { projectId } = req.params;
  const { command, data } = req.body;

  if (!projectStates[projectId]) {
    return res.status(404).json({ error: 'Project not found' });
  }

  try {
    const project = projects[projectId];
    const ueEndpoint = `http://${project.host}:${project.port}/api`;
    
    // Send command to UE5
    const response = await axios.post(`${ueEndpoint}/command`, {
      command,
      data
    }, { timeout: 5000 });

    // Update local state
    updateProjectState(projectId, command, data);

    // Broadcast to all connected clients
    io.emit('state-update', {
      projectId,
      state: projectStates[projectId]
    });

    res.json({ success: true, response: response.data });
  } catch (error) {
    console.error('Command error:', error.message);
    res.status(500).json({ error: error.message });
  }
});

// WebSocket Events
io.on('connection', (socket) => {
  console.log('New client connected:', socket.id);

  // Send current states to new client
  socket.emit('initial-state', projectStates);

  // Handle project selection
  socket.on('select-project', (projectId) => {
    socket.join(`project-${projectId}`);
    socket.emit('project-selected', {
      projectId,
      state: projectStates[projectId]
    });
  });

  // Handle control commands
  socket.on('control-command', async (data) => {
    const { projectId, command, value } = data;
    console.log(`Command received: ${command} = ${value} for project ${projectId}`);

    try {
      const project = projects[projectId];
      if (!project) {
        socket.emit('error', { message: 'Project not found' });
        return;
      }

      const ueEndpoint = `http://${project.host}:${project.port}/api`;
      
      // Send to UE5
      await axios.post(`${ueEndpoint}/command`, {
        command,
        value
      }, { timeout: 5000 });

      // Update state
      updateProjectState(projectId, command, value);

      // Broadcast to all clients in this project
      io.to(`project-${projectId}`).emit('state-update', {
        projectId,
        command,
        value,
        state: projectStates[projectId]
      });

      socket.emit('command-ack', { command, success: true });
    } catch (error) {
      console.error('Control command error:', error.message);
      socket.emit('error', { message: error.message });
    }
  });

  // Handle disconnect
  socket.on('disconnect', () => {
    console.log('Client disconnected:', socket.id);
  });
});

// Helper function to update project state
function updateProjectState(projectId, command, value) {
  const state = projectStates[projectId];
  
  switch (command) {
    case 'sun-rotation':
      state.sunRotation = value;
      break;
    case 'sun-intensity':
      state.sunIntensity = value;
      break;
    case 'day-time':
      state.dayTime = value;
      break;
    case 'scene-change':
      state.currentScene = value;
      break;
    case 'mesh-color':
      state.meshColors[value.meshName] = value.color;
      break;
    case 'sun-position':
      state.sunPosition = value;
      break;
    case 'sun-position-x':
      state.sunPosition.x = value;
      break;
    case 'sun-position-y':
      state.sunPosition.y = value;
      break;
    case 'sun-position-z':
      state.sunPosition.z = value;
      break;
    default:
      console.log('Unknown command:', command);
  }
}

// Health check endpoint
app.get('/health', (req, res) => {
  res.json({ status: 'ok', timestamp: new Date().toISOString() });
});

server.listen(PORT, () => {
  console.log(`🚀 UE5 RenderStream Remote Control Server running on port ${PORT}`);
  console.log(`📱 Open http://localhost:${PORT} in your browser`);
  console.log(`📡 WebSocket server ready for connections`);
  console.log(`🎮 Connected to UE5 projects:`, Object.keys(projects));
});
