// State
let isHalted = false;
let temp = 25.0;
let current = 1.2;
let vibState = false;

// DOM Elements
const tempVal = document.getElementById('temp-val');
const currentVal = document.getElementById('current-val');
const vibVal = document.getElementById('vib-val');
const tempCard = document.getElementById('temp-card');
const vibCard = document.getElementById('vib-card');

const toggleBtn = document.getElementById('toggle-btn');
const statusBanner = document.getElementById('status-banner');
const statusText = document.getElementById('status-text');
const statusPulse = document.getElementById('status-pulse');

// Simulation Logic
function updateSensors() {
    if (isHalted) return;

    // Simulate Temp: Drift around 25-35
    let tempDrift = (Math.random() - 0.4) * 1.5;
    temp += tempDrift;
    
    // Sometimes spike to test alert
    if(Math.random() > 0.95) temp += 4; 
    
    // Bounds
    if(temp < 20) temp = 20;
    if(temp > 45) temp = Math.max(temp - 5, 30);

    // Simulate Current: 0.8A to 4.5A roughly based on temp load
    current = 0.5 + (temp - 20) * 0.15 + (Math.random() * 0.5);

    // Simulate Vibration: Usually LOW, sometimes HIGH when current spikes
    vibState = current > 3.5 && Math.random() > 0.5;

    render();
}

function render() {
    tempVal.innerText = temp.toFixed(1);
    currentVal.innerText = current.toFixed(2);
    
    if(vibState) {
        vibVal.innerText = "HIGH";
        vibVal.style.color = "var(--danger)";
        vibCard.classList.add('alert');
    } else {
        vibVal.innerText = "LOW";
        vibVal.style.color = "var(--success)";
        vibCard.classList.remove('alert');
    }

    // Hardware Alert Simulation
    if (temp > 30.0) {
        tempCard.classList.add('alert');
    } else {
        tempCard.classList.remove('alert');
    }
}

// Toggle Maintenance Mode
toggleBtn.addEventListener('click', () => {
    isHalted = !isHalted;
    
    if (isHalted) {
        // Paused State
        toggleBtn.innerText = "RESUME SYSTEM";
        toggleBtn.className = "btn btn-resume";
        statusText.innerHTML = "⚠ MANUAL QC MODE: SENSORS PAUSED";
        statusBanner.className = "status-banner paused";
        statusPulse.className = "pulse-indicator paused";
    } else {
        // Active State
        toggleBtn.innerText = "HALT FOR MAINTENANCE";
        toggleBtn.className = "btn btn-halt";
        statusText.innerHTML = "Monitoring Active";
        statusBanner.className = "status-banner";
        statusPulse.className = "pulse-indicator";
    }
});

// Run loop
setInterval(updateSensors, 2000);
render(); // Initial render