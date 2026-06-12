/* ===========================
   UE5 RenderStream Remote Control
   Socket.io Client Application
   =========================== */

class RemoteControlApp {
    constructor() {
        this.socket = null;
        this.currentProject = null;
        this.projects = {};
        this.isConnected = false;
        this.debounceTimers = {};
        
        this.init();
    }

    /* ===========================
       Initialization
       =========================== */

    init() {
        this.setupSocket();
        this.setupEventListeners();
        this.loadProjects();
    }

    setupSocket() {
        // Connect to Socket.io server
        this.socket = io({
            reconnection: true,
            reconnectionDelay: 1000,
            reconnectionDelayMax: 5000,
            reconnectionAttempts: 5
        });

        // Connection events
        this.socket.on('connect', () => this.onConnect());
        this.socket.on('disconnect', () => this.onDisconnect());
        this.socket.on('error', (error) => this.onError(error));

        // Data events
        this.socket.on('initial-state', (state) => this.onInitialState(state));
        this.socket.on('state-update', (state) => this.onStateUpdate(state));
        this.socket.on('project-selected', (project) => this.onProjectSelected(project));
        this.socket.on('command-ack', (data) => this.onCommandAck(data));
        this.socket.on('error-message', (error) => this.onErrorMessage(error));
    }

    setupEventListeners() {
        // Lighting controls
        this.setupSliderControl('sunRotation', 'sun-rotation', (value) => value + '°');
        this.setupSliderControl('sunIntensity', 'sun-intensity', (value) => value.toFixed(1));
        this.setupSliderControl('dayTime', 'day-time', (value) => this.formatTime(value));

        // Sun position
        this.setupSliderControl('sunX', 'sun-position-x');
        this.setupSliderControl('sunY', 'sun-position-y');
        this.setupSliderControl('sunZ', 'sun-position-z');

        // Scene selector
        const sceneSelector = document.getElementById('sceneSelector');
        sceneSelector.addEventListener('change', (e) => {
            this.sendCommand('scene-change', e.target.value);
        });

        // Color pickers
        const colorPickers = document.querySelectorAll('.color-picker');
        colorPickers.forEach(picker => {
            picker.addEventListener('change', (e) => {
                const material = e.target.dataset.material;
                this.sendCommand('mesh-color', {
                    meshName: material,
                    color: e.target.value
                });
                this.showToast(`Material ${material} updated to ${e.target.value}`, 'success');
            });
        });

        // Quick action buttons
        document.getElementById('resetBtn').addEventListener('click', () => this.quickAction('reset'));
        document.getElementById('sunriseBtn').addEventListener('click', () => this.quickAction('sunrise'));
        document.getElementById('sunsetBtn').addEventListener('click', () => this.quickAction('sunset'));
        document.getElementById('noonBtn').addEventListener('click', () => this.quickAction('noon'));
    }

    /* ===========================
       Slider Control Helper
       =========================== */

    setupSliderControl(sliderId, commandName, formatter = null) {
        const slider = document.getElementById(sliderId);
        const valueDisplay = document.getElementById(sliderId + 'Value');

        slider.addEventListener('input', (e) => {
            const value = parseFloat(e.target.value);
            valueDisplay.textContent = formatter ? formatter(value) : value;
            
            // Debounce commands to avoid flooding the server
            this.debounceCommand(commandName, value, 100);
        });
    }

    debounceCommand(command, value, delay) {
        if (this.debounceTimers[command]) {
            clearTimeout(this.debounceTimers[command]);
        }

        this.debounceTimers[command] = setTimeout(() => {
            this.sendCommand(command, value);
        }, delay);
    }

    /* ===========================
       Projects Management
       =========================== */

    loadProjects() {
        fetch('/api/projects')
            .then(response => response.json())
            .then(projects => {
                this.projects = projects;
                this.renderProjectsList();
            })
            .catch(error => {
                console.error('Failed to load projects:', error);
                this.showToast('Failed to load projects', 'error');
            });
    }

    renderProjectsList() {
        const projectsList = document.getElementById('projectsList');
        projectsList.innerHTML = '';

        Object.entries(this.projects).forEach(([projectId, project]) => {
            const projectItem = document.createElement('div');
            projectItem.className = 'project-item';
            projectItem.innerHTML = `<i class="fas fa-cube"></i> ${project.name}`;
            
            projectItem.addEventListener('click', () => this.selectProject(projectId));
            projectsList.appendChild(projectItem);
        });
    }

    selectProject(projectId) {
        this.currentProject = projectId;
        this.updateProjectSelection();
        this.socket.emit('select-project', { projectId });
    }

    updateProjectSelection() {
        // Update active state in sidebar
        document.querySelectorAll('.project-item').forEach(item => {
            item.classList.remove('active');
        });

        const items = document.querySelectorAll('.project-item');
        const projectIds = Object.keys(this.projects);
        const activeIndex = projectIds.indexOf(this.currentProject);

        if (activeIndex !== -1) {
            items[activeIndex].classList.add('active');
        }

        // Update panel header
        const project = this.projects[this.currentProject];
        document.getElementById('projectTitle').textContent = project ? project.name : 'Select a Project';
    }

    /* ===========================
       Command Sending
       =========================== */

    sendCommand(command, value) {
        if (!this.isConnected || !this.currentProject) {
            console.warn('Not connected or no project selected');
            return;
        }

        this.socket.emit('control-command', {
            projectId: this.currentProject,
            command,
            value,
            timestamp: Date.now()
        });
    }

    quickAction(action) {
        const actions = {
            'reset': () => {
                this.setSliderValue('sunRotation', 0);
                this.setSliderValue('sunIntensity', 1);
                this.setSliderValue('dayTime', 12);
                this.sendCommand('quick-action', 'reset');
            },
            'sunrise': () => {
                this.setSliderValue('dayTime', 6);
                this.sendCommand('quick-action', 'sunrise');
            },
            'sunset': () => {
                this.setSliderValue('dayTime', 18);
                this.sendCommand('quick-action', 'sunset');
            },
            'noon': () => {
                this.setSliderValue('dayTime', 12);
                this.sendCommand('quick-action', 'noon');
            }
        };

        if (actions[action]) {
            actions[action]();
            this.showToast(`${action.charAt(0).toUpperCase() + action.slice(1)} executed`, 'success');
        }
    }

    setSliderValue(sliderId, value) {
        const slider = document.getElementById(sliderId);
        slider.value = value;
        slider.dispatchEvent(new Event('input'));
    }

    /* ===========================
       Socket.io Event Handlers
       =========================== */

    onConnect() {
        this.isConnected = true;
        this.updateConnectionStatus(true);
        console.log('Connected to server');
        this.showToast('Connected to server', 'success');
    }

    onDisconnect() {
        this.isConnected = false;
        this.updateConnectionStatus(false);
        console.log('Disconnected from server');
        this.showToast('Disconnected from server', 'error');
    }

    onError(error) {
        console.error('Socket error:', error);
        this.showToast('Connection error: ' + error, 'error');
    }

    onInitialState(state) {
        console.log('Received initial state:', state);
        this.updateControlsFromState(state);
    }

    onStateUpdate(state) {
        console.log('State updated:', state);
        this.updateControlsFromState(state);
    }

    onProjectSelected(project) {
        console.log('Project selected:', project);
        this.showToast(`Connected to ${project.name}`, 'info');
    }

    onCommandAck(data) {
        console.log('Command acknowledged:', data);
    }

    onErrorMessage(error) {
        console.error('Server error:', error);
        this.showToast('Error: ' + error.message, 'error');
    }

    updateControlsFromState(state) {
        if (state.sunRotation !== undefined) {
            this.setSliderValue('sunRotation', state.sunRotation);
        }
        if (state.sunIntensity !== undefined) {
            this.setSliderValue('sunIntensity', state.sunIntensity);
        }
        if (state.dayTime !== undefined) {
            this.setSliderValue('dayTime', state.dayTime);
        }
        if (state.sunPosition) {
            if (state.sunPosition.x !== undefined) {
                this.setSliderValue('sunX', state.sunPosition.x);
            }
            if (state.sunPosition.y !== undefined) {
                this.setSliderValue('sunY', state.sunPosition.y);
            }
            if (state.sunPosition.z !== undefined) {
                this.setSliderValue('sunZ', state.sunPosition.z);
            }
        }
        if (state.scene) {
            document.getElementById('sceneSelector').value = state.scene;
        }
    }

    /* ===========================
       UI Helper Functions
       =========================== */

    updateConnectionStatus(connected) {
        const statusIndicator = document.getElementById('connectionStatus');
        const statusText = document.getElementById('statusText');

        if (connected) {
            statusIndicator.classList.remove('offline');
            statusIndicator.classList.add('online');
            statusText.textContent = 'Connected';
        } else {
            statusIndicator.classList.remove('online');
            statusIndicator.classList.add('offline');
            statusText.textContent = 'Disconnected';
        }
    }

    formatTime(hours) {
        const h = Math.floor(hours);
        const m = Math.round((hours - h) * 60);
        return `${String(h).padStart(2, '0')}:${String(m).padStart(2, '0')}`;
    }

    showToast(message, type = 'info') {
        const toastContainer = document.getElementById('toastContainer');
        const toast = document.createElement('div');
        
        toast.className = `toast ${type}`;
        toast.textContent = message;
        
        toastContainer.appendChild(toast);

        // Auto-remove after 4 seconds
        setTimeout(() => {
            toast.classList.add('removing');
            setTimeout(() => {
                toast.remove();
            }, 300);
        }, 4000);
    }
}

/* ===========================
   Application Start
   =========================== */

document.addEventListener('DOMContentLoaded', () => {
    console.log('Initializing UE5 Remote Control App...');
    new RemoteControlApp();
});

/* ===========================
   Service Worker Registration
   (Optional - for offline support)
   =========================== */

if ('serviceWorker' in navigator) {
    window.addEventListener('load', () => {
        navigator.serviceWorker.register('/sw.js')
            .then(registration => {
                console.log('Service Worker registered:', registration);
            })
            .catch(error => {
                console.log('Service Worker registration failed:', error);
            });
    });
}

/* ===========================
   Error Handling
   =========================== */

window.addEventListener('error', (event) => {
    console.error('Global error:', event.error);
});

window.addEventListener('unhandledrejection', (event) => {
    console.error('Unhandled promise rejection:', event.reason);
});
