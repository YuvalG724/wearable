# 🩺 Smart Watch Based Health Monitoring System

> **Course:** Embedded C Programming — BECE320E  
> **Institution:** VIT  

---

## 👥 Team Members

Yuval Gupta, 
Srijita Bhattacharjee, 
Ananya Deo
---

## 📖 Overview

A low-cost, ESP32-based wearable health monitoring system that tracks **heart rate** and **step count** in real time, displaying results on an OLED screen. Built as a proof-of-concept alternative to commercial smartwatches using off-the-shelf embedded components.

---

## ✨ Features

- 💓 **Heart Rate Monitoring** — Pulse sensor reads analog signals and maps them to BPM values
- 🚶 **Step Detection** — MPU6050 accelerometer uses threshold-based detection to count steps
- 📊 **Real-Time OLED Display** — Shows heart rate, acceleration, gyroscope values, and step count
- ⚡ **Low Cost** — Total BOM cost ~₹900
- 🔌 **I2C Bus Sharing** — MPU6050 and OLED share SDA/SCL lines to minimize wiring

---

## 🔧 Hardware Components

| Component | Function |
|-----------|----------|
| ESP32 Microcontroller | Central processing unit |
| Pulse Sensor | Analog heart rate input |
| MPU6050 (Accelerometer + Gyroscope) | Motion and step detection |
| OLED Display | Real-time data output |

---

## 🏗️ System Architecture

```
┌──────────────┐     Analog      ┌─────────────┐     I2C      ┌─────────────┐
│  Pulse Sensor│ ──────────────► │             │ ───────────► │    OLED     │
└──────────────┘                 │    ESP32    │              │   Display   │
                                 │             │              └─────────────┘
┌──────────────┐     I2C         │             │
│   MPU6050    │ ──────────────► │             │
└──────────────┘                 └─────────────┘
   (Accel + Gyro)
```

**Data Flow:** Sense → Process → Display

---

## 💻 Software & Tools

- **IDE:** Arduino Platform IDE
- **Language:** Embedded C
- **Communication Protocol:** I2C (for MPU6050 and OLED)
- **Step Detection:** Acceleration magnitude threshold-based algorithm
- **BPM Calculation:** Linear scaling from raw pulse sensor analog values

---

## ⚙️ How It Works

### Heart Rate
1. Pulse sensor outputs an analog signal to an ESP32 ADC pin
2. Raw analog values are linearly scaled to approximate BPM
3. BPM is updated and displayed on the OLED in real time

### Step Detection
1. MPU6050 sends raw acceleration data over I2C
2. ESP32 calculates acceleration magnitude:  
   `|a| = √(ax² + ay² + az²)`
3. If `|a|` exceeds a set threshold and timing conditions are met → step is counted

### Display Refresh
The OLED periodically refreshes with:
- Current heart rate (BPM)
- X/Y/Z acceleration values
- Gyroscope readings
- Total step count

---

## 🧪 Testing & Validation

| Test | Condition | Result |
|------|-----------|--------|
| Step count (idle) | User stationary | Count unchanged ✅ |
| Step count (walking) | User moving | Count increases ✅ |
| Heart rate | Finger placed on sensor | BPM variation observed ✅ |
| Integration | All modules active | System functions correctly ✅ |

---

## 🚀 Future Work

- [ ] Add second MPU6050 for **exercise rep counting** (push-ups, squats, curls)
- [ ] Implement **signal filtering** (e.g., moving average / Kalman filter) to reduce sensor noise
- [ ] Integrate **Bluetooth / Wi-Fi** for mobile app data transmission
- [ ] Add **SpO₂ sensor** for blood oxygen monitoring
- [ ] Add **temperature sensor** for body temperature tracking
- [ ] Redesign into a more **compact wearable form factor** with optimized power consumption

---

## 📁 Output / Demo

🔗 [Google Drive — Project Output](https://drive.google.com/drive/folders/1X61HGTxRy88MrHTE_DaB6-P3NNwsro3k)

---

*© 2024 Yuval Gupta, Srijita Bhattacharjee, Ananya Deo — VIT | BECE320E*
