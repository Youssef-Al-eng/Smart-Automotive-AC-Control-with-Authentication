# Automotive Air Conditioning Control Panel with Authentication 

**Embedded Systems Final Project**

## 📌 Author

  - **Abdalah Mohamed**
  - **Amr Ahmed**
  - **Youssef Alaa**

**Status:** Final Release  
**Version:** 1.0  
**Date:** 09/12/2025

-----

## 📖 Introduction

This project implements a multi-functional **AC Control & Environmental Monitoring System**. It features a robust **Finite State Machine (FSM)** architecture that manages authentication (Face ID simulation & Password), distinct AC control modes (Voice, Manual, Automatic), and real-time sensor monitoring using an **Arduino/AVR**, **DC Motor**, **LCD**, and various sensors (**LM35, MQ2, LDR**).

The system ensures security via a login screen and provides user interaction through a **4x4 Keypad**, **Push Buttons**, **UART commands**, and a **16x2 LCD** interface.

-----

## 🎯 Purpose

The system demonstrates:

  - **Finite State Machine (FSM)** design for complex menu navigation.
  - **Pulse Width Modulation (PWM)** for DC motor speed control.
  - **ADC Sensor Interfacing** (Temperature, Gas, Light).
  - **UART Communication** for simulating advanced features (Face ID, Voice Control).
  - **Layered Architecture** separating Hardware Abstraction Layers (HAL) from Application logic.

-----

## 📌 Scope

This project includes:

  - **Authentication:** Dual-option login via Face ID (UART signal) or Keypad Password.
  - **Modes:**
      - **Monitor:** Real-time display of Temp, Gas, and Light levels.
      - **Voice AC:** Control motor speed via serial text commands.
      - **Keypad AC:** Manual 0-100% speed setting.
      - **Auto AC:** PID-like control attempting to match a set temperature.
  - **Hardware Abstraction:** Modular drivers for Motor, LCD, Keypad, and Sensors.

The project **does not** include onboard image processing (Face ID is simulated via UART trigger) or actual audio processing (Voice commands are text-based via UART).

-----

```text
============================================================
               SYSTEM ARCHITECTURE
============================================================

                 ┌─────────────────────────┐
                 │   APPLICATION LAYER     │
                 │       (App.ino)         │
                 │------------------------ │
                 │ - State Machine Logic   │
                 │ - Menu Navigation       │
                 │ - Auth Handling         │
                 │ - AC Control Algorithms │
                 └───────────┬──────────── ┘
                             │
                             │ uses
                             ▼
       ┌─────────────────────────────────────────────┐
       │       HAL LAYER (Hardware Abstraction)      │
       │---------------------------------------------│
       │ ┌──────────────┐ ┌──────────────┐           │
       │ │ Display      │ │ DC Motor     │           │
       │ │ Display.h    │ │ DC_Motor.h   │           │
       │ └──────────────┘ └──────────────┘           │
       │ ┌──────────────┐ ┌──────────────┐           │
       │ │ Sensors      │ │ Inputs       │           │
       │ │ LM35/MQ2/LDR │ │ Keypad/Btn   │           │
       │ └──────────────┘ └──────────────┘           │
       └───────────┬───────────────────┬───────────  ┘
                   │                   │
                   │ uses              │ uses
                   ▼                   ▼
         ┌─────────────────┐   ┌─────────────────┐
         │  Motor / Sensors│   │  LCD / Keypad   │
         └─────────────────┘   └─────────────────┘

                             ▲
                             │
                             │ depends on
                             │
       ┌─────────────────────────────────────────────┐
       │           UTILITIES LAYER                   │
       │---------------------------------------------│
       │ • std_types.h → Standard types (uint8 etc.) │
       │ • PWM.h       → Timer Configuration         │
       │ • uart.h      → Serial Communication        │
       │ • Timer.h     → Millis/Time tracking        │
       └─────────────────────────────────────────────┘

============================================================
                      DATA / CONTROL FLOW
============================================================

Sensors (LM35/MQ2) ──>  HAL Drivers  ──┐
UART (PC/Script)   ──>  Serial Buffer ─┤
Keypad / Buttons   ──>  Input Drivers ─┤
                                       ▼
                             Application Layer
                           (State Machine Switch)
                                       │
         ┌─────────────────────────────┴────────────────────────┐
         │                             │                        │
   Actuation Outputs             Visual Output            Debug/Status
   ┌───────────────┐            ┌─────────────┐          ┌──────────────┐
   │ DC Motor (Fan)│            │ LCD Display │          │ UART TX      │
   │ (PWM Speed)   │            └─────────────┘          │ (Log msgs)   │
   └───────────────┘                                     └──────────────┘

============================================================
```

-----

## ✔️ System Behavior

### State Machine Overview

| State | Description |
|----------|---------------|
| **STATE\_AUTH\_MENU** | Main entry. Select Face ID or Password login. |
| **STATE\_FACE\_WAIT** | Waits for `SYSTEM_INIT` command via UART (External Script). |
| **STATE\_PASS\_ENTRY**| Accepts 4-digit PIN via Keypad. (Default: "1111"). |
| **STATE\_MENU** | Main dashboard to select Monitor, Voice, Manual, or Auto modes. |
| **STATE\_MONITOR** | Cyclically updates LCD with LM35, MQ2, and LDR readings. |
| **STATE\_AUTO\_AC** | Adjusts Fan Speed automatically based on Setpoint vs Ambient Temp. |

### Control Logic

**1. Voice Mode (UART):**

  * Receive `AC_50` → Set Motor PWM to 50% duty cycle.
  * Receive `reverse` → Reverse Motor direction.

**2. Auto AC Mode:**

  * User sets target temp (e.g., 24°C).
  * System compares Target vs Actual.
  * `Diff < 0`: Fan OFF.
  * `Diff 1-3`: Low Speed.
  * `Diff > 5`: Max Speed.

-----

## 🔧 Hardware Components

| Component | Purpose |
|----------|---------|
| Arduino/AVR | Main microcontroller |
| LM35 Sensor | Temperature measurement |
| MQ2 Sensor | Gas/Smoke detection |
| LDR Module | Light intensity measurement |
| 16×2 LCD | User Interface display |
| 4x4 Keypad | Password entry and numeric input |
| 3 Push Buttons | Menu navigation (Up, Down, Select) |
| DC Motor + Driver | AC Fan simulation (L298N or similar) |
| UART Interface | PC Communication (USB) |

-----

## 🔌 Circuit Connections

*Note: Specific pin mappings are defined in the individual header files (`DC_Motor.h`, `LCD.h`, etc.).*

### Inputs

| Component | Connection |
|----------|-------------|
| LM35 / MQ2 / LDR | Analog Input Pins (ADC) |
| 4x4 Keypad | Digital Row/Col Pins |
| Navigation Buttons | Digital Input (Pull-up) |
| UART RX | RX Pin (0) |

### Outputs

| Component | Connection |
|----------|-----|
| DC Motor Enable | PWM Pin (Timer Output) |
| DC Motor In1/In2 | Digital Output |
| LCD Data/Control | Digital Output Port |
| UART TX | TX Pin (1) |

-----

## 💻 Software Overview

### 1\. Authentication

  * **Face ID:** The system enters a blocking wait state listening for `"SYSTEM_INIT"` over Serial. This simulates a Python script detecting a face and sending a signal.
  * **Password:** Users enter digits via Keypad. If the buffer matches the hardcoded hash ("1111"), access is granted.

### 2\. Sensor Monitoring

  * Uses non-blocking timers (`millis()`) to poll sensors every 3 seconds.
  * Converts ADC values to human-readable units:
      * `Temp = ADC * resolution`
      * Gas/Light = Raw ADC mapping.

### 3\. Motor Control (PWM)

  * Uses a `map()` function to convert percentage inputs (0-100%) to 8-bit PWM values (0-255).
  * Supports directional control (Forward/Reverse/Stop).

### 4\. Main Loop Tasks

1.  **Read Serial:** Check for incoming commands (Voice/Face ID).
2.  **State Handler:** Execute logic for the current `SystemState`.
3.  **UI Update:** Refresh LCD only when data changes to prevent flickering.

-----
### System Features

| Feature                | Description |
|------------------------|-------------|
| Dual Authentication    | Secure access via Password or Biometric simulation |
| Smart Auto-Control     | Fan speed adjusts dynamically based on temperature delta |
| Voice Simulation       | Control AC speed using text commands over UART |
| Safety Limits          | Motor speed capped at 100%; temperature range limited (18–30°C) |
| Live Monitoring        | Continuous tracking of environmental data (Gas, Light, Temperature) |
| Debug Interface        | Real-time system status logging via UART |
