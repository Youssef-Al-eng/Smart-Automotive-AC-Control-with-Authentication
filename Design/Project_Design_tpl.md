| **Group Memebrs**  | `Amr Ahmed, Youssef Alaa, Abdalah Mohamed` |
| :---------- | :------------------------------------------ |
| **Status**  | `Final Project Report`                  |
| **Version** | `1.0`                                       |
| **Date**    | `09/12/2025`                                |

---
## Introduction
Modern embedded systems are increasingly required to combine intelligent sensing, user friendly interfaces, and adaptive control strategies into a single cohesive platform.
This project integrates multiple functionalities: authentication, environmental monitoring, voice interaction, keypad input, and automated actuation into a complete, real-time microcontroller-based system.

By using sensors such as the LM35, MQ2, and LDR and a DC motor control, an LCD interface, a keypad, and voice-based UART communication, the system demonstrates how a microcontroller can serve as the core of a many model control environment.

The design aims to emphasize functional diversity, modularity, and scalability. Each subsystem is implemented as a separate driver, to make sure the operations are  efficient and enabling future expansion and upgrades.


### Purpose
The primary purpose of this project is to **design and implement a smart, user-interactive environmental control system** that provides flexible authentication, real-time monitoring, and intelligent motor-based cooling/ventilation control.

The system aims to:

* Provide secure authentication through **face recognition** or **PIN entry**.
* Display real-time measurements of **temperature**, **gas concentration**, and **ambient light level**.
* Allow users to control the AC motor using:

  * **Voice commands**
  * **Keypad numerical input**
  * **Automatic temperature-based logic**
* Improve comfort and safety by adjusting motor speed based on environmental conditions.
* Demonstrate the integration of multiple hardware and software modules in a modular embedded architecture.

Ultimately, the system highlights how embedded controllers can support **multi-input control strategies** and **autonomous decision-making**, while remaining simple and intuitive for the user.


### Scope
The scope of this project includes the complete **design, implementation, integration, and testing** of a modular embedded control system.

### **Included Functionalities**

* Two authentication methods:

  1. **Face detection** via external serial communication
  2. **Keypad PIN entry**
* Multi-option menu navigation using hardware buttons.
* Monitoring mode showing:

  * Temperature (LM35)
  * Gas level (MQ2)
  * Light intensity (LDR)
* AC motor control through:

  * **Voice commands (0%, 25%, 50%, 75%, 100%, reverse)**
  * **Keypad input**
  * **Automatic speed control** based on temperature difference

### **Hardware Scope**

* Sensors: **LM35**, **MQ2**, **LDR**
* Actuators: **DC motor** (PWM controlled)
* Interfaces: **16×2 LCD**, **3×4 keypad**, **push buttons**
* Communication: **UART** for face detection and voice command processing

### **Software Scope**

* Modular driver development:

  * LCD, keypad, buttons, sensors, motor, PWM, UART
* A structured **state machine** controlling authentication, menus, and modes
* Clean HAL abstraction with reusable modules
* Complete error handling and user feedback through LCD & Serial Monitor

### **Out of Scope**

* Full HVAC system control
* On-board face recognition processing
* Wireless communication, cloud integration, or mobile app support

---

## Architectural Overview

@startuml
title Architecture Overview

rectangle "System Architecture" as SYS #FFE5CC {

  ' =======================
  ' High-Level Layer (Application)
  ' =======================
  rectangle "High-Level Layer\n(Application Layer)" as APP_LAYER #FFCC99 {
      rectangle "Application Logic\n(app.ino)" as APP
  }

  ' =======================
  ' Driver Layer (HAL)
  ' =======================
  rectangle "Driver Layer\n(HAL - Hardware Abstraction Layer)" as HAL_LAYER #CCFFCC {
      rectangle "LCD Driver\n(LCD.h / LCD.ino)" as LCD_DRV
      rectangle "DC Motor Driver\n(DC_Motor.h / DC_Motor.ino)" as MOTOR_DRV
      rectangle "PWM Driver\n(PWM.h / PWM.ino)" as PWM_DRV
      rectangle "LM35 Temp Sensor Driver\n(LM35.h / LM35.ino)" as LM35_DRV
      rectangle "MQ2 Gas Sensor Driver\n(MQ2.h / MQ2.ino)" as MQ2_DRV
      rectangle "LDR Light Sensor Driver\n(LDR.h / LDR.ino)" as LDR_DRV
      rectangle "Buttons Driver\n(Buttons.h / button.ino)" as BTN_DRV
      rectangle "Keypad Driver\n(key.h / key.ino)" as KPD_DRV
      rectangle "UART Wrapper\n(uart.h / uart.ino)" as UART_DRV
      rectangle "Display Helpers\n(Display.h / Display.ino)" as DISP_DRV
      rectangle "System Timer Driver\n(Timer.h / Timer.ino)" as TMR_DRV
  }

  ' =======================
  ' MCAL Layer
  ' =======================
  rectangle "MCAL Layer\n(Microcontroller Abstraction Layer)" as MCAL_LAYER #CCFFFF {
      rectangle "ADC Module\n(ADC0/1/2 for LM35,\nLDR, MQ2)" as ADC_MCAL
      rectangle "Timer5 PWM Module\n(OC5C on Pin 44)" as TMR5_MCAL
      rectangle "Timer0 System Tick\n(CTC, 1 ms tick)" as TMR0_MCAL
      rectangle "Digital I/O (DIO)\n(PORTA/B/C/D/L etc.)" as DIO_MCAL
      rectangle "USART0 Module\n(HW Serial)" as USART_MCAL
  }

  ' =======================
  ' Common Utilities
  ' =======================
  rectangle "Common Utilities" as UTIL_LAYER #FFF2CC {
      rectangle "Bitwise Macros\n(macros.h)" as MACROS
      rectangle "Standard Types\n(std_types.h)" as TYPES
  }

  ' =======================
  ' Physical Hardware Layer
  ' =======================
  rectangle "Physical Layer\n(Hardware Components)" as HW_LAYER #E6E6E6 {
      rectangle "Arduino Mega 2560\n(MCU Board)" as MEGA
      rectangle "DC Motor + Fan Blades\n(Through H-Bridge / L298N)" as MOTOR_HW
      rectangle "LM35 Temperature Sensor\n(PF0 / ADC0)" as LM35_HW
      rectangle "MQ2 Gas Sensor\n(PF2 / ADC2)" as MQ2_HW
      rectangle "LDR Sensor\n(PF1 / ADC1)" as LDR_HW
      rectangle "16x2 LCD\n(4-bit interface)" as LCD_HW
      rectangle "4x Menu Push Buttons\n(UP/DOWN/SELECT/RESET)" as BTN_HW
      rectangle "3x4 Keypad\n(PL0–PL3 rows,\nPL4–PL6 cols)" as KPD_HW
      rectangle "External 8V Battery\n+ 5V Regulator (5V rail)" as PWR_HW
  }

  ' =======================
  ' External Host (Face + Voice)
  ' =======================
  rectangle "External Host\n(PC / Laptop)" as HOST #F0F0FF {
      rectangle "Face Recognition\n(SYSTEM_INIT over UART)" as FACE_PC
      rectangle "Voice Commands\n(\"AC_25\", \"reverse\", ...)" as VOICE_PC
  }

  ' =======================
  ' Relationships: Application -> Drivers
  ' =======================
  APP -down-> LCD_DRV
  APP -down-> MOTOR_DRV
  APP -down-> PWM_DRV
  APP -down-> LM35_DRV
  APP -down-> MQ2_DRV
  APP -down-> LDR_DRV
  APP -down-> BTN_DRV
  APP -down-> KPD_DRV
  APP -down-> UART_DRV
  APP -down-> DISP_DRV
  APP -down-> TMR_DRV

  ' =======================
  ' Relationships: Drivers -> MCAL
  ' =======================
  LM35_DRV -down-> ADC_MCAL
  MQ2_DRV  -down-> ADC_MCAL
  LDR_DRV  -down-> ADC_MCAL

  PWM_DRV    -down-> TMR5_MCAL
  MOTOR_DRV  -down-> TMR5_MCAL
  MOTOR_DRV  -down-> DIO_MCAL

  LCD_DRV  -down-> DIO_MCAL
  BTN_DRV  -down-> DIO_MCAL
  KPD_DRV  -down-> DIO_MCAL

  UART_DRV -down-> USART_MCAL

  TMR_DRV  -down-> TMR0_MCAL

  ' =======================
  ' Relationships: Drivers -> Utilities
  ' =======================
  LCD_DRV   -down-> MACROS
  MOTOR_DRV -down-> MACROS
  PWM_DRV   -down-> MACROS
  BTN_DRV   -down-> MACROS
  KPD_DRV   -down-> MACROS
  TMR_DRV   -down-> MACROS

  LM35_DRV  -down-> TYPES
  MQ2_DRV   -down-> TYPES
  LDR_DRV   -down-> TYPES
  UART_DRV  -down-> TYPES
  BTN_DRV   -down-> TYPES
  KPD_DRV   -down-> TYPES
  DISP_DRV  -down-> TYPES
  TMR_DRV   -down-> TYPES

  ' =======================
  ' MCAL -> Physical MCU
  ' =======================
  ADC_MCAL   -down-> MEGA
  TMR5_MCAL  -down-> MEGA
  TMR0_MCAL  -down-> MEGA
  DIO_MCAL   -down-> MEGA
  USART_MCAL -down-> MEGA

  MACROS -down-> MEGA
  TYPES  -down-> MEGA

  ' =======================
  ' MCU -> Sensors / Actuators / Power
  ' =======================
  MEGA -down-> LM35_HW
  MEGA -down-> MQ2_HW
  MEGA -down-> LDR_HW
  MEGA -down-> LCD_HW
  MEGA -down-> BTN_HW
  MEGA -down-> KPD_HW
  MEGA -down-> MOTOR_HW
  MEGA -down-> PWR_HW

  ' =======================
  ' External Host via UART
  ' =======================
  MEGA -right-> HOST : UART0 (TX/RX)
  HOST -down-> FACE_PC
  HOST -down-> VOICE_PC
}

@enduml


---

## Assumptions and Constraints

### **Assumptions**

* The system is expected to run on a stable 5V regulated supply, and the external battery/regulator combination provides constant output without sudden dips.
* All sensors (LM35, MQ2, LDR) are assumed to be calibrated well enough for non-critical monitoring, not for industrial-grade precision.
* The user interacting with the device understands basic navigation through buttons and keypad input.
* The external host (PC/Laptop) running face-detection and voice-command software is assumed to send accurate and clean serial messages (e.g., `SYSTEM_INIT`, `AC_50`, `reverse`) without corruption.
* The keypad hardware provides clean, bounce-free signals after software debouncing, and no mechanical faults exist.
* The DC motor and driver are assumed to handle the PWM ranges used in this project without thermal issues.
* The environment where the system operates stays within normal indoor conditions, meaning no excessive humidity, dust, or extreme temperatures that would damage sensors.
* Users will only enter valid temperature ranges on purpose and will follow the general input rules (e.g., using `*` to clear and `#` to confirm).
* The system is assumed to run continuously without requiring resets, except when the user explicitly presses the reset button.

---

### **Constraints**

* The entire system is limited by the processing speed and memory footprint of the ATmega2560, restricting the use of heavy algorithms or full-scale AI models on the microcontroller itself.
* ADC readings are constrained by 10-bit resolution (0–1023), limiting sensor accuracy compared to industrial equipment.
* The DC motor speed range is bound by the capabilities of Timer5 PWM resolution and the physical response curve of the motor (i.e., not perfectly linear).
* Face detection and voice command processing must rely on an external computer, since the microcontroller cannot run such models natively.
* The keypad uses only 3 columns and 4 rows, meaning the system is constrained to the standard 0–9, `*`, and `#` layout with no room for special keys.
* User interface feedback is limited to a 16×2 character LCD, restricting how much information can be displayed at once.
* The system depends heavily on accurate timing from the Timer0 module (1ms ticks); any timer malfunction directly affects periodic tasks like monitoring and motor control.
* The MQ2 and LDR sensors have slow response times by nature, meaning rapid environmental changes cannot be captured instantly.
* All real-time decisions are confined to a single main loop, so long operations or delays must be carefully handled to avoid blocking other system features.
* The system is limited to UART0 for communication, which means only one external serial device (PC/Laptop) can be interfaced at a time.


---
### Main Loop Behavior


#### Diagram 1 (Authentication Flow)

@startuml
title Authentication Flow (Face or Password)

start

:Power on / Reset;
:Initialize LCD, Buttons, PWM, Motor,
LM35, MQ2, LDR, Keypad, Serial;
:Show Auth Menu on LCD
("1. Face Detection", "2. Password");

if (User selects\nFace Detection?) then (Yes)
  :Clear LCD;
  :Show "Face Detection";
  :Send "Waiting for face..." over Serial;

  if ("SYSTEM_INIT"\nreceived on Serial?) then (Yes)
    :Face verified;
    :Show "Face Recognized, Access Granted!";
    :Go to Main Menu (STATE_MENU);
    stop
  else (No)
    :Stay in STATE_FACE_WAIT;
    :Keep checking Serial
    and RESET button;
  endif

else (No)
  if (User selects\nPassword?) then (Yes)
    :Clear LCD;
    :Show "Enter Password";
    :Reset password buffer;

    :User types digits\non Keypad;

    if ('*' pressed?) then (Yes)
      :Clear password buffer;
      :Refresh password line;
    endif

    if ('#' pressed?) then (Yes)
      if (Password == "1111"?) then (Yes)
        :Show "Access Granted!";
        :Send success over Serial;
        :Go to Main Menu (STATE_MENU);
        stop
      else (No)
        :Show "Wrong Password";
        :Reset password buffer;
      endif
    endif

  endif
endif

if (RESET button pressed?) then (Yes)
  :Return to Auth Menu\n(STATE_AUTH_MENU);
endif

stop
@enduml

#### Diagram 2 (Main Menu Navigation)
@startuml
title Main Menu Navigation

start

:Enter Main Menu (STATE_MENU)
after successful auth;
:Show items on LCD:
1. Monitor Sys
2. Voice AC Ctrl
3. Keypad AC Set
4. Set Temp AC;

:Read buttons (UP, DOWN, SELECT);

if (UP pressed?) then (Yes)
  :Move selection up;
  :Update '>' cursor\non LCD;
endif

if (DOWN pressed?) then (Yes)
  :Move selection down;
  :Update '>' cursor\non LCD;
endif

if (SELECT pressed?) then (Yes)
  if ("1. Monitor Sys"\ncurrently selected?) then (Yes)
    :Set state = STATE_MONITOR;
    :Enter Monitor System mode;
    stop
  else (No)
    if ("2. Voice AC Ctrl"\ncurrently selected?) then (Yes)
      :Set state = STATE_VOICE_AC;
      :Enter Voice AC mode;
      stop
    else (No)
      if ("3. Keypad AC Set"\ncurrently selected?) then (Yes)
        :Set state = STATE_SET_TEMP;
        :Enter Keypad AC mode;
        stop
      else (No)
        :Set state = STATE_SET_TEMP_SENSOR;
        :Enter Temp-Based AC mode;
        stop
      endif
    endif
  endif
endif

:If no SELECT, stay\nin Main Menu
and keep reading buttons;

stop
@enduml

#### Diagram 3 (System Modes Overview)

@startuml
title System Modes (Monitor, Voice AC, Keypad AC, Temp-Based AC)

start

:Enter selected mode from Main Menu;

if (Mode == "Monitor Sys"?) then (Monitor)

  partition "Monitor System (STATE_MONITOR)" {
    :Every 3 seconds:\n- Read LM35 (Temp)\n- Read MQ2 (Gas)\n- Read LDR (Light);
    :Update LCD:\nLine1: T:xx.xC\nLine2: G:xxx L:xxx;
    :Send same data over Serial;

    if (RESET button pressed?) then (Yes)
      :Stop monitoring and\nreturn to Main Menu;
      stop
    else (No)
      :Continue periodic monitoring;
    endif
  }

elseif (Mode == "Voice AC Ctrl"?) then (Voice)

  partition "Voice AC Control (STATE_VOICE_AC)" {
    :Show "Voice Mode AC" and current\nspeed (%) on LCD;
    :Read Serial command;

    if (Command == "reverse"?) then (Yes)
      :motor_backward(255)\n(100% reverse);\nShow "Motor Backward";\nLog over Serial;
    elseif (Command starts with "AC_"?) then (Yes)
      :Parse XX from "AC_XX" as\n0, 25, 50, 75 or 100;
      :Map XX% → PWM\n(0/25/50/75/100 → 0/low/med/high/max);\nCall motor_forward(PWM);\nUpdate "Speed: XX%" on LCD;\nLog over Serial;
    endif

    if (RESET button pressed?) then (Yes)
      :motor_stop();\nReturn to Main Menu;
      stop
    else (No)
      :Stay in Voice AC mode;
    endif
  }

elseif (Mode == "Keypad AC Set"?) then (Keypad)

  partition "Keypad AC Control (STATE_SET_TEMP)" {
    :Prompt "Enter AC Speed:" on LCD;
    :User types one of 0,25,50,75,100\non the keypad;

    if ('*' pressed?) then (Yes)
      :motor_backward(255)\n(100% reverse);\nShow "Motor Backward";\nLog over Serial;\nClear input buffer;
    endif

    if ('#' pressed?) then (Yes)
      :Parse typed value as %;\nIf not 0/25/50/75/100 → clamp/ignore;
      :Map % → PWM\n(0/25/50/75/100 → 0/low/med/high/max);\nCall motor_forward(PWM);\nShow "Set Speed: XX%";\nLog over Serial;\nClear input buffer;
    endif

    if (RESET button pressed?) then (Yes)
      :motor_stop();\nReturn to Main Menu;
      stop
    else (No)
      :Remain in Keypad AC mode;
    endif
  }

elseif (Mode == "Automatic AC Control"?) then (User enters the temp)

  partition "Temp-Based AC Control (STATE_SET_TEMP_SENSOR)" {
    :On first entry:\nShow "Waiting..." then clear;
    :User enters target temp (18–30°C)\nvia keypad;

    if ('*' pressed?) then (Yes)
      :Clear input buffer and\nre-prompt "Set Temp:";
    endif

    if ('#' pressed?) then (Yes)
      if (Input == 99?) then (Reverse)
        :motor_backward(255)\n(100% reverse);\nShow "Motor Backward";\nLog over Serial;
      else (Normal Set)
        :Clamp set_temp to 18–30°C;\nShow "Temp Set: XXC";\nLog target temp over Serial;
      endif
      :Clear input buffer;
    endif

    :Every 3 seconds:\nRead current_temp from LM35;\nCompute diff = current_temp - set_temp;

    if (diff <= 0°C?) then (≤ 0)
      :Speed = 0%;
    elseif (diff <= 3.0°C?) then (0–3)
      :Speed = 25%;
    elseif (diff <= 7.5°C?) then (3–7.5)
      :Speed = 50%;
    elseif (diff <= 10.0°C?) then (7.5–10)
      :Speed = 75%;
    else (diff > 10°C)
      :Speed = 100%;
    endif

    :Map Speed (0/25/50/75/100)\n→ PWM (0/low/med/high/max);
    :If not in 99-reverse mode:\nSpeed=0% → motor_stop()\nelse → motor_forward(PWM);

    :Update LCD:\nLine1: Set:XXC Cur:YY.YC\nLine2: Diff:DD.D PWM:NN;
    :Log set_temp, current_temp,\ndiff and PWM over Serial;

    if (RESET button pressed?) then (Yes)
      :motor_stop();\nReturn to Main Menu;
      stop
    else (No)
      :Continue temp-based control;
    endif
  }


endif

@enduml


---

## Functional Description

This chapter describes the complete functional behavior of the integrated **Authentication, Monitoring, and AC Control System**. The system uses a state-based architecture, where each major feature authentication, monitoring, and AC control operates within its own mode and transitions according to user interactions or sensor inputs.
The software coordinates keypad inputs, sensor readings, serial communication, motor control, and LCD display updates to deliver a fully interactive and intelligent embedded solution.

---

### **1. System Overview**

When powered ON, the system begins with an authentication stage. The user must choose one of two verification methods:

* **Face Recognition** (via Serial communication with an external AI/Python script)
* **Password Entry** (via keypad)

Once authentication succeeds, the system displays the **Main Menu**, which includes four operating modes:

1. **System Monitoring**
2. **Voice-Controlled AC Mode**
3. **Keypad-Controlled AC Mode**
4. **Temperature-Based Automatic AC Mode**

Navigation is performed using four hardware buttons:

* **UP**
* **DOWN**
* **SELECT**
* **RESET**

Each mode has unique functionality and dedicated logic to control sensors, motor speed, and LCD feedback.

---

### **2. Authentication Functions**

#### **Face Recognition**

* The Arduino waits for a message from the external face-recognition script.
* When “SYSTEM_INIT” is received, the face is considered verified.
* LCD shows confirmation, and the system transitions to the main menu.
* RESET returns to the authentication screen.

#### **Password Entry**

* User enters a 4-digit password using the numeric keypad.
* `*` clears all entered digits.
* `#` confirms the password.
* If correct (“1111”), access is granted; otherwise, an error message is shown.
* RESET returns to the authentication screen.

---

### **3. Main Menu Navigation**

The system shows a scrollable menu with four options.
Due to the 16×2 LCD limitation, only two items are displayed at once.

Navigation behavior:

* **UP button:** Move one item upward
* **DOWN button:** Move downward
* **SELECT button:** Enter selected mode
* **RESET button:** Exit to authentication at any time

---

### **4. System Monitoring Mode**

This mode provides real-time environmental feedback using:

* **LM35** → Temperature
* **MQ2** → Gas/smoke concentration
* **LDR** → Light intensity

Every 3 seconds:

1. The system reads the sensors
2. LCD displays:

   * Line 1 → `T:xx.xC`
   * Line 2 → `G:xxx L:xxx`
3. Values are sent over Serial for debugging or external logging

RESET returns to the main menu.

---

### **5. Voice-Controlled AC Mode**

This mode receives AC control commands from an external speech-recognition engine over Serial.

#### Supported Commands

* **“reverse”** → Motor runs backward at full PWM
* **“AC_0”**, **“AC_25”**, **“AC_50”**, **“AC_75”**, **“AC_100”**

#### PWM Mapping

| Voice Command | Motor Speed (%) | PWM Duty |
| ------------- | --------------- | -------- |
| AC_0          | 0%              | 0        |
| AC_25         | 25%             | 120      |
| AC_50         | 50%             | 170      |
| AC_75         | 75%             | 210      |
| AC_100        | 100%            | 255      |

The LCD always reflects the current operating speed.
RESET stops the motor and returns to the menu.

---

### **6. Keypad-Controlled AC Mode**

This mode allows manual control of AC speed using the keypad:

#### Behavior

* User enters a numeric value **0–100**
* Press `#` → Apply entered AC level
* Press `*` → Reverse motor at full speed
* LCD shows the selected value and corresponding PWM

RESET stops the motor and returns to menu.

---

### **7. Temperature-Based Automatic AC Mode**

This mode enables intelligent automatic AC behavior based on:

* User's **desired temperature** (set via keypad)
* Continuous LM35 readings
* Difference between both values (**ΔT = current_temp – set_temp**)

#### Temperature Difference Logic

| Difference ΔT (°C) | Motor Speed (%) |
| ------------------ | --------------- |
| ΔT ≤ 0             | 0% (OFF)        |
| 0–3                | 25%             |
| 3–7.5              | 50%             |
| 7.5–10             | 75%             |
| >10                | 100%            |

#### Special Code

* Entering **99** triggers reverse ventilation mode.

#### Every 3 seconds:

* Read LM35
* Compute ΔT
* Select PWM based on the table above
* Update LCD:

  * Line 1 → `Set:X Cur:Y.Y`
  * Line 2 → `Diff:D.D PWM:N`
* Log all values to Serial

RESET returns to the main menu.

---

### **8. Error Handling & Safety Measures**

* Keypad inputs automatically reset after invalid or out-of-range entries
* Serial buffer is cleared after every processed command
* PWM values are clamped to prevent unsafe motor speeds
* RESET button always returns the system to a safe idle state
* Reverse mode automatically overrides all other outputs for safety and ventilation

---

### **9. System State Architecture**

The system operates as a **finite state machine (FSM)**:

* **STATE_AUTH_MENU**
* **STATE_FACE_WAIT**
* **STATE_PASSWORD_ENTRY**
* **STATE_MENU**
* **STATE_MONITOR**
* **STATE_VOICE_AC**
* **STATE_SET_TEMP**
* **STATE_SET_TEMP_SENSOR**

This ensures:

* Clean transitions
* Easy debugging
* Predictable program behavior
* Good modular separation of all features

---



## Implementation of the Module

### Hardware Implementation

This chapter explains how the hardware modules of the system are wired around the **Arduino Mega 2560** and how each component (sensors, keypad, LCD, DC motor, and buttons) is physically connected to the microcontroller.

---

### 1. Components Used

| **Component**                       | **Description**                                                                                       | **Quantity** |
| ----------------------------------- | ----------------------------------------------------------------------------------------------------- | ------------ |
| Arduino Mega 2560                   | Main microcontroller board running the full authentication, monitoring, and AC control firmware.      | 1            |
| 16×2 LCD (4-bit mode)               | Character display used to show temperature, gas level, light intensity, menu items, and motor status. | 1            |
| LM35 temperature sensor             | Analog temperature sensor (10 mV/°C) used to measure cabin/room temperature.                          | 1            |
| MQ2 gas sensor                      | Analog gas/smoke sensor used to estimate CO₂ / smoke concentration levels.                            | 1            |
| LDR (light-dependent resistor)      | Light sensor used to measure ambient light intensity.                                                 | 1            |
| DC motor + 3-blade fan head         | Main actuator representing the AC blower; speed and direction are controlled electronically.          | 1            |
| H-bridge / motor driver             | Interface between Arduino and DC motor (direction pins + PWM input for speed control).                | 1            |
| 3×4 keypad                          | Matrix keypad used for PIN entry, manual AC speed control, and target temperature input.              | 1            |
| Push buttons (UP/DOWN/SELECT/RESET) | Four standalone buttons for navigating and controlling the system menu.                               | 4            |
| External DC supply                  | Powers the motor driver and Arduino (e.g., lab power supply / adapter).                               | 1            |
| Flyback diode (if discrete driver)  | Protects driver circuitry from inductive motor spikes (if not integrated in the module).              | 1            |
| Resistors                           | For LEDs (if used), pull-ups (when needed), and sensor conditioning if required.                      | Several      |
| Breadboard / PCB                    | Used to assemble and route the circuit connections.                                                   | 1            |
| Jumper wires                        | Wires for interconnecting sensors, keypad, LCD, motor driver, and Arduino pins.                       | Several      |

> **Note:** Although visually it looks like a “fan”, electrically it is a **DC motor** with a 3-blade fan head mounted on its shaft, driven via an H-bridge using direction pins and a PWM speed input.

---

### 2. Circuit Description

#### 2.1 Power and Ground

* The **Arduino Mega 2560** is powered from an external DC source (e.g. adapter or lab supply).
* The **motor driver** is powered from the same or a higher-current supply line, depending on the motor’s rating.
* All modules (Arduino, sensors, keypad, driver, LCD) share a **common GND reference**.

---

#### 2.2 Sensors (LM35, MQ2, LDR)

All three analog sensors are connected to the **ADC inputs on Port F**.

* **LM35 – Temperature Sensor**

  * VCC → +5 V
  * GND → GND
  * Output → **ADC0 / PF0**
  * Code reference: `LM35_ReadADC(0)` in `LM35.ino`.

* **LDR – Light Sensor**

  * Configured in a voltage divider with a resistor.
  * Junction of divider → **ADC1 / PF1**
  * Code reference: `ADMUX = ... | 1;` in `LDR_Read()`.

* **MQ2 – Gas Sensor**

  * VCC → +5 V
  * GND → GND
  * Analog output → **ADC2 / PF2**
  * Code reference: `ADMUX = ... | 2;` in `MQ2_Read()`.

All three modules use **AVCC** as the ADC reference, configured in their respective `*_Init()` functions.

---

#### 2.3 LCD Interface (16×2, 4-bit Mode)

The LCD is driven in 4-bit mode using **Port A** and **Port B**:

* **Control Pins**

  * `RS` → **PB0**
  * `EN` → **PB1**
  * Configured in `LCD_Init()` as outputs via `LCD_RS_DDR` and `LCD_EN_DDR`.

* **Data Pins (D4–D7 in 4-bit mode)**

  * D4 → **PA0**
  * D5 → **PA1**
  * D6 → **PA2**
  * D7 → **PA3**
  * Set as outputs via `LCD_DATA_DDR`.

* The LCD uses:

  * Command instructions (`LCD_Command`) for cursor/clear/entry mode.
  * Data writes (`LCD_Char`, `LCD_String`) to display text.
  * Positioning via `LCD_SetCursor(row, col)`.

---

#### 2.4 Navigation Buttons (UP, DOWN, SELECT, RESET)

The four hardware navigation buttons are connected to **Port C**:

* Defined in `Buttons.h` as:

  * **UP**    → `PC7`
  * **DOWN**  → `PC6`
  * **SELECT**→ `PC5`
  * **RESET** → `PC4`

* `buttons_init()`:

  * Configures these pins as **inputs** (`BTN_DDR &= ~...`).
  * Enables **internal pull-ups** (`BTN_PORT |= ...`).

* A pressed button pulls the pin **LOW**, and `read_button()` maps:

  * `1` → UP
  * `2` → DOWN
  * `3` → SELECT
  * `4` → RESET

These pins are used throughout `Final.ino` to navigate authentication menus, main menu, and to exit modes.

---

#### 2.5 Keypad (3×4 Matrix)

The 3×4 keypad is implemented using **Port L** bits `PL0–PL6`, as defined in `key.h`:

* **Rows (Outputs):**

  * R1 → `PL0`
  * R2 → `PL1`
  * R3 → `PL2`
  * R4 → `PL3`

* **Columns (Inputs with pull-ups):**

  * C1 → `PL4`
  * C2 → `PL5`
  * C3 → `PL6`

In `Keypad_Init()`:

* Rows (PL0–PL3) are configured as **outputs**.
* Columns (PL4–PL6) are configured as **inputs with internal pull-ups**.

In `Keypad_GetKey()`:

* The code drives one row LOW at a time and checks each column input:

  * If a column reads LOW, it means the key at that (row, column) is pressed.
  * The corresponding character is returned from `keys[4][3]` (digits, `*`, `#`).

The keypad is used to:

* Enter the **password** (1111) in `handle_password_entry()`.
* Enter AC **speed** in **Keypad AC mode** (`handle_set_temp()`).
* Enter **target temperature** in **Temperature-Based AC mode** (`handle_set_temp_sensor()`).

---

#### 2.6 DC Motor with Fan Head + PWM (AC Blower)

The DC motor (with the 3-blade fan cover attached) is driven by an H-bridge / motor driver using:

* **Direction Pins** (from `DC_Motor.h`):

  * `MOTOR_PIN1` → **PD2**
  * `MOTOR_PIN2` → **PD3**
  * Configured as outputs in `motor_init()`.

* **PWM Speed Control**:

  * PWM output on **PL5** → Arduino Mega **Pin 44**
  * Defined in `PWM.h` as `PWM_PIN = PL5`.
  * Controlled via Timer 5, Channel C (`OCR5C`) in `PWM.ino`.

The motor driver connections are:

* EN (enable / PWM input) → Arduino Mega pin 44 (PL5 PWM).
* IN1, IN2 (direction inputs) → PD2, PD3 respectively.

Thus:

* **Forward rotation** (cooling mode):

  * PD2 = HIGH, PD3 = LOW
  * PWM duty = 0–255 (speed proportional).

* **Reverse rotation** (ventilation / smoke extraction):

  * PD2 = LOW, PD3 = HIGH
  * PWM duty = 255 (full power) or according to mode.

The motor is used across all AC-related modes: **Voice AC**, **Keypad AC**, and **Temperature-Based AC**.

---

### 3. Circuit Connections Summary

| **Module / Component** | **Arduino Port/Pin(s)**         | **Function / Type**                          |
| ---------------------- | ------------------------------- | -------------------------------------------- |
| LM35 Output            | `PF0` / ADC0                    | Analog temperature input                     |
| MQ2 Output             | `PF2` / ADC2                    | Analog gas/smoke input                       |
| LDR Output             | `PF1` / ADC1                    | Analog light intensity input                 |
| LCD Data (D4–D7)       | `PA0–PA3`                       | 4-bit data output lines                      |
| LCD RS                 | `PB0`                           | Register Select control                      |
| LCD EN                 | `PB1`                           | Enable control                               |
| UP Button              | `PC7`                           | Digital input with internal pull-up          |
| DOWN Button            | `PC6`                           | Digital input with internal pull-up          |
| SELECT Button          | `PC5`                           | Digital input with internal pull-up          |
| RESET Button           | `PC4`                           | Digital input with internal pull-up          |
| Keypad Rows R1–R4      | `PL0–PL3`                       | Row outputs for scanning                     |
| Keypad Columns C1–C3   | `PL4–PL6`                       | Column inputs with internal pull-ups         |
| Motor Direction 1      | `PD2`                           | H-bridge input 1 (forward/backward control)  |
| Motor Direction 2      | `PD3`                           | H-bridge input 2 (forward/backward control)  |
| Motor PWM (Speed)      | `PL5` / Pin 44 (OCR5C)          | PWM output to motor driver enable            |
| Serial (UART0)         | Arduino USB (TX0/RX0)           | Communication with PC / face-recognition app |
| Common Ground          | GND                             | Shared reference for all modules             |
| Power Input            | External DC supply → 5 V / Vmot | Power for logic and motor driver             |

---

### 4. Working Principle

1. **Sensors & Inputs**

   * The **LM35**, **MQ2**, and **LDR** continuously provide analog readings to the ADC.
   * The **keypad** and **buttons** provide user input for authentication and mode selection.

2. **Processing & Control**

   * The **Arduino Mega** runs `Final.ino` as the main control program:

     * Handles authentication (face or password).
     * Navigates the main menu with the four buttons.
     * Reads sensors periodically in monitoring mode.
     * Interprets user voice commands (via Serial strings) and keypad input.
     * Computes the required PWM duty cycle based on mode (manual %, or temperature difference).

3. **Output & Feedback**

   * The **16×2 LCD** shows menus, sensor values, AC speed, target temp, and current temp/difference.
   * The **DC motor with fan head** changes speed and direction according to:

     * Voice commands (`AC_0/25/50/75/100`, `reverse`)
     * Keypad-entered percentage
     * Automatic rules based on temperature difference between **setpoint** and **actual**.

4. **Ventilation vs Cooling**

   * In **forward direction**, the motor simulates normal AC cooling airflow.
   * In **reverse direction**, it simulates ventilation/extraction to clear smoke or fog inside the vehicle/cabin.

5. **Safety & Reset**

   * At any time, the **RESET button**:

     * Stops the motor.
     * Exits the current mode.
     * Returns the system to a safe, known state (main menu or authentication).




---

## Integration and Configuration

### Static Files

| **File name**                 | **Contents**                                                                                                                                                                                                                                                                                                 |
| ----------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `app.ino`                     | Main application file. Implements the overall state machine (authentication menu, face/password verification, main menu, Monitor, Voice AC, Keypad AC, Temp-Based AC), calls all drivers, and uses `Timer_GetMillis()` for non-blocking periodic updates.                                                    |
| `LCD.ino` / `LCD.h`           | LCD driver in 4-bit mode. Handles pin configuration, initialization sequence, command sending, character and string printing, cursor positioning, and line clearing (`LCD_ClearLine`).                                                                                                                       |
| `DC_Motor.ino` / `DC_Motor.h` | DC motor driver. Configures H-bridge pins, and provides `motor_init()`, `motor_forward(speed)`, `motor_backward(speed)` and `motor_stop()` functions. Uses PWM to control motor speed.                                                                                                                       |
| `PWM.ino` / `PWM.h`           | PWM driver using Timer5 Channel C (OC5C on Pin 44). Initializes Fast PWM 8-bit mode and exposes `pwm_init()` and `pwm_set_duty(uint8_t duty)` for speed control of the DC motor.                                                                                                                             |
| `LM35.ino` / `LM35.h`         | Temperature sensor driver for the LM35 on ADC0 (PF0). Configures the ADC and provides `LM35_Init()` and `LM35_Read()` to return the ambient temperature in °C.                                                                                                                                               |
| `MQ2.ino` / `MQ2.h`           | Gas sensor driver for the MQ2 on ADC2 (PF2). Initializes the ADC reference and prescaler and exposes `MQ2_Init()` / `MQ2_Read()` to obtain raw gas level readings.                                                                                                                                           |
| `LDR.ino` / `LDR.h`           | Light sensor driver for the LDR on ADC1 (PF1). Provides `LDR_Init()` / `LDR_Read()` to measure ambient light intensity as a raw ADC value.                                                                                                                                                                   |
| `button.ino` / `Buttons.h`    | Menu buttons driver. Configures four push buttons (UP, DOWN, SELECT, RESET) on PORTC with internal pull-ups, and exposes `buttons_init()` and `read_button()` for high-level navigation input.                                                                                                               |
| `key.ino` / `key.h`           | 3×4 keypad driver on PORTL (rows PL0–PL3, columns PL4–PL6). Handles row scanning, column reading with pull-ups, debouncing, and provides `KEYPAD_Init()` and `KEYPAD_GetKey()` for numeric and control input (`*`, `#`).                                                                                     |
| `Timer.ino` / `Timer.h`       | System tick timer module. Configures Timer0 in CTC mode to generate a 1 ms interrupt, increments a global millisecond counter, and exposes `Timer_Init()` and `Timer_GetMillis()` as a replacement for `millis()`.                                                                                           |
| `Display.ino` / `Display.h`   | UI helper module built on top of the LCD driver. Provides higher-level functions like `update_temp_display()`, `update_limits_display()`, and `display_message()` to format and refresh user-facing information. *(Used mainly in previous coursework; some helpers may be reused or partially referenced.)* |
| `uart.ino` / `uart.h`         | UART abstraction layer for USART0. Provides `Uart_Init()`, `Uart_SetBaudRate()`, `Uart_SendChar()`, `Uart_ReadData()`, and `Uart_SendString()` to interface with an external host (PC) for face detection and voice commands. Currently prepared for integration in place of the Arduino `Serial` API.       |
| `macros.h`                    | Common bitwise and utility macros (e.g. `SET_BIT`, `CLEAR_BIT`, `TOGGLE_BIT`, `BIT_IS_SET`) used across drivers for low-level register manipulation and cleaner I/O code.                                                                                                                                    |
| `std_types.h`                 | Standard type definitions (`uint8`, `uint16`, `uint32`, `sint8`, `float_32`, etc.) used in all modules to improve readability, consistency, and portability of the embedded C code.                                                                                                                          |

---


---
## Include Structure

@startuml
title Final project - Smart AC Control & Monitoring Project\nFile Dependency Overview

package "Source Code Structure" {

    ' ==== Main Application ====
    [app.ino] --> [Display.h]
    [app.ino] --> [DC_Motor.h]
    [app.ino] --> [PWM.h]
    [app.ino] --> [Buttons.h]
    [app.ino] --> [LM35.h]
    [app.ino] --> [MQ2.h]
    [app.ino] --> [LDR.h]
    [app.ino] --> [key.h]
    [app.ino] --> [Timer.h]
    [app.ino] --> [std_types.h]

    ' ==== LCD & Display ====
    [LCD.ino] --> [LCD.h]
    [LCD.ino] --> [std_types.h]
    [LCD.ino] --> [macros.h]

    [Display.ino] --> [Display.h]
    [Display.ino] --> [LCD.h]
    [Display.ino] --> [std_types.h]

    ' ==== Sensors ====
    [LM35.ino] --> [LM35.h]
    [LM35.ino] --> [std_types.h]
    [LM35.ino] --> [macros.h]

    [MQ2.ino] --> [MQ2.h]
    [MQ2.ino] --> [std_types.h]
    [MQ2.ino] --> [macros.h]

    [LDR.ino] --> [LDR.h]
    [LDR.ino] --> [std_types.h]
    [LDR.ino] --> [macros.h]

    ' ==== Input Devices ====
    [Buttons.ino] --> [Buttons.h]
    [Buttons.ino] --> [std_types.h]

    [key.ino] --> [key.h]
    [key.ino] --> [std_types.h]

    ' ==== Motor & PWM ====
    [DC_Motor.ino] --> [DC_Motor.h]
    [DC_Motor.ino] --> [PWM.h]
    [DC_Motor.ino] --> [macros.h]
    [DC_Motor.ino] --> [std_types.h]

    [PWM.ino] --> [PWM.h]
    [PWM.ino] --> [macros.h]
    [PWM.ino] --> [std_types.h]

    ' ==== Timer ====
    [Timer.ino] --> [Timer.h]
    [Timer.ino] --> [std_types.h]
}

@enduml


---

## Configuration Parameters


| **Name**       | **Default / Fixed Value** | **Description**                                              |
| -------------- | ------------------------- | ------------------------------------------------------------ |
| `AUTH_OPTIONS` | 2                         | Number of authentication methods (Face Detection, Password). |
| `PASSWORD`     | `"1111"`                  | Default 4-digit password for keypad authentication.          |

---

### System Timing

| **Name**                  | **Value** | **Description**                                                |
| ------------------------- | --------- | -------------------------------------------------------------- |
| `SYSTEM_TICK`             | 1 ms      | Base timing resolution generated by **Timer0** using CTC mode. |
| `UPDATE_INTERVAL_MONITOR` | 3000 ms   | Interval for refreshing LM35, MQ2, and LDR readings.           |
| `UPDATE_INTERVAL_TEMP_AC` | 3000 ms   | Interval to recompute PWM based on temperature difference.     |
| `_delay_ms(1)`            | 1 ms      | Loop micro-delay used in all state transitions.                |

---

### Temperature-Based AC Thresholds

| **Name**               | **Value** | **Description**                                                       |
| ---------------------- | --------- | --------------------------------------------------------------------- |
| `SET_TEMP_MIN`         | 18 °C     | Minimum user-settable temperature.                                    |
| `SET_TEMP_MAX`         | 30 °C     | Maximum user-settable temperature.                                    |
| `SPECIAL_CODE_REVERSE` | `99`      | When entered from keypad, the AC motor runs in reverse at full speed. |

---

### PWM Mapping

| **Name**      | **PWM Value** | **When Used**                                                  |
| ------------- | ------------- | -------------------------------------------------------------- |
| `PWM_0`       | 0             | Motor OFF.                                                     |
| `PWM_25`      | 120           | 25% speed (Voice AC mode).                                     |
| `PWM_50`      | 170           | 50% speed.                                                     |
| `PWM_75`      | 210           | 75% speed.                                                     |
| `PWM_100`     | 255           | 100% speed (max).                                              |
| `PWM_REVERSE` | 255           | Used when the user requests reverse airflow (Voice or Keypad). |

---

### Temperature-Difference Rules (Set Temp AC Mode)

| **Temperature Difference (°C)** | **Motor PWM Output** |
| ------------------------------- | -------------------- |
| `diff <= 0°C`                   | 0% (Motor OFF)       |
| `0 < diff <= 3°C`               | 25% speed            |
| `3 < diff <= 7.5°C`             | 50% speed            |
| `7.5 < diff <= 10°C`            | 75% speed            |
| `diff > 10°C`                   | 100% speed           |

---

### Sensor Configuration (MCU Inputs)

| **Sensor**           | **MCU Pin / ADC Channel** | **Description**                    |
| -------------------- | ------------------------- | ---------------------------------- |
| **LM35 Temp Sensor** | PF0 / ADC0                | Reads temperature in °C.           |
| **LDR Light Sensor** | PF1 / ADC1                | Measures ambient light level.      |
| **MQ2 Gas Sensor**   | PF2 / ADC2                | Detects combustible gases / smoke. |

---

### Motor & PWM Configuration

| **Name**         | **Pin**      | **Description**                    |
| ---------------- | ------------ | ---------------------------------- |
| `MOTOR_PIN1`     | PD2          | Motor direction control (Forward). |
| `MOTOR_PIN2`     | PD3          | Motor direction control (Reverse). |
| `PWM_PIN (OC5C)` | PL5 / Pin 44 | Hardware PWM output (Timer5).      |

---

### LCD Configuration (4-bit Mode)

| **Signal** | **Pin** | **Description**  |
| ---------- | ------- | ---------------- |
| `LCD_RS`   | PB0     | Register Select. |
| `LCD_EN`   | PB1     | Enable strobe.   |
| `LCD_D4`   | PA0     | Data bit 4.      |
| `LCD_D5`   | PA1     | Data bit 5.      |
| `LCD_D6`   | PA2     | Data bit 6.      |
| `LCD_D7`   | PA3     | Data bit 7.      |

---

### Button Configuration (Menu Navigation)

| Function     | Pin | Mode                       |
| ------------ | --- | -------------------------- |
| UP           | PC7 | Digital input with pull-up |
| DOWN         | PC6 | Digital input with pull-up |
| SELECT       | PC5 | Digital input with pull-up |
| RESET / BACK | PC4 | Digital input with pull-up |

---

### Keypad Configuration (4×3)

| Row/Column | MCU Pin (Port L) | Description       |
| ---------- | ---------------- | ----------------- |
| `ROW1`     | PL0              | Output (row scan) |
| `ROW2`     | PL1              | Output            |
| `ROW3`     | PL2              | Output            |
| `ROW4`     | PL3              | Output            |
| `COL1`     | PL4              | Input (pull-up)   |
| `COL2`     | PL5              | Input (pull-up)   |
| `COL3`     | PL6              | Input (pull-up)   |

---

### UART / External Host (Face + Voice Recognition)

| Purpose          | Description                                                                     |
| ---------------- | ------------------------------------------------------------------------------- |
| Face Recognition | Waits for `"SYSTEM_INIT"` from a PC or Python script.                           |
| Voice AC Control | Receives `"AC_0"`, `"AC_25"`, `"AC_50"`, `"AC_75"`, `"AC_100"`, or `"reverse"`. |
| UART Module      | USART0 hardware registers (TX0/RX0).                                            |

---

### Power Configuration

| Component                | Value                                        |
| ------------------------ | -------------------------------------------- |
| Arduino Mega 2560        | Powered through onboard 5V regulator         |
| DC Motor                 | External 8V–12V supply via H-bridge / driver |
| Sensors (LM35, LDR, MQ2) | 5V supply from MCU                           |

---

## Conclusion

The development of this integrated monitoring and control system brought together multiple sensing, actuation, and user-interaction modules into a single, cohesive embedded solution. What began as a simple collection of components temperature, gas, and light sensors, a DC motor, a keypad, and a set of menu buttons ultimately evolved into a structured, state-driven system capable of handling authentication, environment monitoring, and multiple modes of AC control with a surprisingly intuitive user flow.

Throughout the implementation, the project focused on reliability and responsiveness. For example, the introduction of a dedicated timer module helped us avoid blocking delays and use precise, non-intrusive timekeeping. This improved how the menu navigated and how sensors updated, making the system feel more refined and purposeful. The layers built around each hardware module, such as the LCD, keypad, sensors, motor, PWM, and buttons, kept the application logic clear, readable, and easy to maintain, even as we added more features.

The dual authentication path demonstrated how different technologies can coexist within the same embedded platform. Users were given the choice between face verification via external serial communication and a traditional keypad password interface, allowing flexibility without complicating the underlying architecture. Similarly, the AC control modes showcased different interaction styles such as a voice controlled PWM levels, keypad based motor commands, and a fully automated temperature regulated mode. Each mode served a real functional purpose while reinforcing the modularity of the design.

This project highlights the value of building systems that are both modular and scalable. Because every feature was implemented as a discrete driver or state, the system can now grow naturally whether by integrating predictive algorithms, enhancing authentication security, adding connectivity, or expanding into multi-zone environmental control. The foundations laid here ensure that future improvements can be layered on without redesigning the entire system from scratch.

In the end, the project stands as a practical demonstration of how hardware, software, and user experience come together in embedded design. It balances structure with flexibility, and functionality with clarity providing a solid baseline for more advanced, intelligent systems to be developed in the future.

---



