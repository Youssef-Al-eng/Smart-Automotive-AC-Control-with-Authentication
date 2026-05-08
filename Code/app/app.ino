/************************************************************************************
 * Module: AC Control System
 * File Name: App.ino
 * Description: Main Loop for Embedded AC Control System with LCD, Face ID, 
 *              Password, Voice Control, Keypad Control, and Sensor-Based Temp
 * Author: Abdalah Mohamed
 ************************************************************************************/

#include "std_types.h"     // Standard type definitions (uint8_t, int16_t, etc.)
#include "Display.h"       // LCD display functions
#include "DC_Motor.h"      // DC motor control functions
#include "PWM.h"           // PWM initialization and control
#include "Buttons.h"       // Button reading functions
#include "LM35.h"          // Temperature sensor functions
#include "MQ2.h"           // Gas sensor functions
#include "LDR.h"           // Light sensor functions
#include "key.h"           // Keypad input functions
#include "Timer.h"         // Timer utility for millis
#include "uart.h"          // UART serial communication
#include <util/delay.h>    // Delay functions
#include <stdio.h>         // Standard I/O for sprintf
#include <string.h>        // String manipulation functions
#include <stdlib.h>        // Conversion functions (atoi, atof)

/* Map function prototype */
long map(long x, long in_min, long in_max, long out_min, long out_max);

// ==========================================
// System States Definition
// ==========================================
typedef enum {
    STATE_AUTH_MENU,         // Authentication selection menu (Face or Password)
    STATE_FACE_WAIT,         // Waiting for face recognition result
    STATE_PASSWORD_ENTRY,    // Password entry state
    STATE_MENU,              // Main system menu
    STATE_MONITOR,           // Sensor monitoring state
    STATE_VOICE_AC,          // Voice-controlled AC state
    STATE_SET_TEMP,          // Keypad-controlled AC speed setting
    STATE_SET_TEMP_SENSOR    // Automatic AC control based on temperature sensor
} SystemState;

SystemState currentState = STATE_AUTH_MENU; // Current system state

// ==========================================
// State Entry Flags
// Each flag ensures the LCD or UART messages are printed only once per entry
// ==========================================
uint8_t face_entry_flag = 1;
uint8_t pass_entry_flag = 1;
uint8_t mon_entry_flag = 1;
uint8_t voice_entry_flag = 1;
uint8_t keypad_entry_flag = 1;
uint8_t auto_entry_flag = 1;

// ==========================================
// Authentication Menu Setup
// ==========================================
#define AUTH_OPTIONS 2
const char* authMenu[AUTH_OPTIONS] = {
    "1. Face Detection",
    "2. Password"
};
int8_t authIndex = 0;  // Currently selected option
int8_t authTop = 0;    // Top visible option on LCD

// ==========================================
// Main Menu Setup
// ==========================================
#define TOTAL_OPTIONS 4
const char* menuItems[TOTAL_OPTIONS] = {
    "1. Monitor Sys",
    "2. Voice AC Ctrl",
    "3. Keypad AC Set",
    "4. Set Temp AC"
};
int8_t menuIndex = 0;   // Selected menu item
int8_t topIndex = 0;    // Top visible menu item on LCD

// ==========================================
// Global Buffers
// ==========================================
char lcdBuffer[17];         // LCD display buffer
char serialBuffer[32];      // UART command buffer
uint8_t serialIdx = 0;      // Current UART buffer index
volatile uint8_t commandReady = 0;  // Flag indicating UART command is ready
char uart_msg_buffer[64];   // Buffer for UART messages

// ==========================================
// Mock Variables for Testing
// ==========================================
float mock_temp_val = 0.0;  // Mock temperature value
uint8_t use_mock_temp = 0;  // Flag to use mock temperature
const char* PASSWORD = "1111";  // Password for keypad login

// ==========================================
// Function Prototypes
// ==========================================
void system_init(void);         // Initialize all peripherals
void read_serial_command(void); // Read command from UART
void reset_entry_flags(void);   // Reset all state entry flags

// Handlers for different system states
void show_auth_menu(void);
void handle_auth_menu(void);
void handle_face_wait(void);
void handle_password_entry(void);
void show_menu_display(void);
void handle_menu(void);
void handle_monitor(void);
void handle_voice_ac(void);
void handle_set_temp(void);
void handle_set_temp_sensor(void);

// ==========================================
// Main Function
// ==========================================
int main(void) {
    system_init();  // Initialize all peripherals and system
    while (1) {
        read_serial_command();  // Continuously read UART commands
        switch(currentState) {  // State machine handler
            case STATE_AUTH_MENU:        handle_auth_menu();        break;
            case STATE_FACE_WAIT:        handle_face_wait();        break;
            case STATE_PASSWORD_ENTRY:   handle_password_entry();   break;
            case STATE_MENU:             handle_menu();             break;
            case STATE_MONITOR:          handle_monitor();          break;
            case STATE_VOICE_AC:         handle_voice_ac();         break;
            case STATE_SET_TEMP:         handle_set_temp();         break;
            case STATE_SET_TEMP_SENSOR:  handle_set_temp_sensor();  break;
        }
        _delay_ms(1); // Small delay to avoid tight looping
    }
}

// ==========================================
// Initialization Function
// Sets up all modules and prints ready message
// ==========================================
void system_init(void) {
    LCD_Init(); 
    buttons_init();
    pwm_init();
    motor_init();
    LM35_Init();
    MQ2_Init();
    LDR_Init();
    KEYPAD_Init();  
    Timer_Init(); 
    Uart_Init();

    LCD_Clear();
    show_auth_menu(); // Show authentication menu at start
    
    _delay_ms(100);
    sprintf(uart_msg_buffer, "SYS_READY\r\n");
    Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
    
    _delay_ms(50);
    sprintf(uart_msg_buffer, "MENU: AUTH\r\n");
    Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
}

// Reset all entry flags so next state prints messages
void reset_entry_flags(void) {
    face_entry_flag = 1;
    pass_entry_flag = 1;
    mon_entry_flag = 1;
    voice_entry_flag = 1;
    keypad_entry_flag = 1;
    auto_entry_flag = 1;
}

// ==========================================
// UART Command Handling
// ==========================================
void read_serial_command(void) {
    if (commandReady) return;  // Skip if previous command not processed
    while (Uart_IsDataAvailable()) {
        char c = Uart_ReadData();
        if (c == '\n' || c == '\r') { // End of command
            if(serialIdx > 0) { 
                serialBuffer[serialIdx] = '\0';
                serialIdx = 0;  
                commandReady = 1; // Mark command ready
                return;
            }
        } else {
            if (serialIdx < 31) serialBuffer[serialIdx++] = c; // Store char
        }
    }
}

// ==========================================
// Authentication Menu Handlers
// ==========================================
void show_auth_menu(void) {
    LCD_Clear();
    LCD_SetCursor(0, 0);
    if(authIndex == authTop) LCD_Char('>'); else LCD_Char(' '); // Cursor indicator
    LCD_String(authMenu[authTop]);

    if(authTop + 1 < AUTH_OPTIONS) { // Show next option if exists
        LCD_SetCursor(1, 0);
        if(authIndex == authTop + 1) LCD_Char('>'); else LCD_Char(' ');
        LCD_String(authMenu[authTop + 1]);
    }
}

// Handle button navigation in auth menu
void handle_auth_menu(void) {
    uint8_t btn = read_button();
    if (btn == 1) {  // UP button
        sprintf(uart_msg_buffer, "EVENT: UP_BTN\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        if(authIndex > 0) { authIndex--; if(authIndex < authTop) authTop = authIndex; show_auth_menu(); _delay_ms(200);}
    } 
    else if (btn == 2) { // DOWN button
        sprintf(uart_msg_buffer, "EVENT: DOWN_BTN\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        if(authIndex < AUTH_OPTIONS-1) { authIndex++; if(authIndex > authTop+1) authTop=authIndex-1; show_auth_menu(); _delay_ms(200);}
    }
    else if (btn == 3) { // SELECT button
        sprintf(uart_msg_buffer, "EVENT: SELECT_BTN\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        _delay_ms(20); 

        LCD_Clear();
        serialIdx = 0; serialBuffer[0] = '\0'; commandReady = 0;
        reset_entry_flags();

        if(authIndex==0) { // Face detection
            currentState = STATE_FACE_WAIT; 
            sprintf(uart_msg_buffer, "STATE: FACE_WAIT\r\n");
            Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        }
        else { // Password
            currentState = STATE_PASSWORD_ENTRY;
            sprintf(uart_msg_buffer, "STATE: PASSWORD_ENTRY\r\n");
            Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        }
    }
}

// ==========================================
// Face Waiting State
// ==========================================
void handle_face_wait(void) {
    if(face_entry_flag) { 
        LCD_Clear(); LCD_String("Waiting Face..."); 
        sprintf(uart_msg_buffer, "FACE: WAITING\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        face_entry_flag = 0; 
    }

    if (commandReady) { 
        if (strcmp(serialBuffer, "SYSTEM_INIT") == 0) { // Simulated access granted
            LCD_Clear(); LCD_String("Access Granted!");
            sprintf(uart_msg_buffer, "FACE: ACCESS_GRANTED\r\n");
            Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
            _delay_ms(1500);
            
            menuIndex = 0; topIndex = 0; show_menu_display(); 
            currentState = STATE_MENU; reset_entry_flags();

            sprintf(uart_msg_buffer, "MENU: MAIN\r\n");
            Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        }
        serialBuffer[0] = '\0'; commandReady = 0;
    }
}

// ==========================================
// Password Entry State
// Logic for entering PIN via 4x4 Keypad
// ==========================================
void handle_password_entry(void) {
    static char passBuffer[5]; // Holds the entered 4 digits + null
    static uint8_t index=0;    // Current digit position

    // 1. Initial UI Setup
    if(pass_entry_flag) {
        LCD_Clear(); LCD_SetCursor(0,0); LCD_String("Enter Password:");
        LCD_SetCursor(1,0); LCD_String("_"); 
        index=0; passBuffer[0]='\0'; pass_entry_flag = 0; 
    }

    // 2. Scan Keypad
    char key = KEYPAD_GetKey();
    if(key) {
        // Log keypress to UART
        sprintf(uart_msg_buffer, "KEY: %c\r\n", key);
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));

        if(key>='0' && key<='9' && index<4) { // It's a number and buffer not full
            passBuffer[index++]=key; passBuffer[index]='\0';
            // Display asterisk for security
            LCD_SetCursor(1,index-1); LCD_Char('*');
        } 
        else if(key=='#') {  // ENTER Key
            sprintf(uart_msg_buffer, "EVENT: SUBMIT\r\n");
            Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
            
            // Check Password
            if(strcmp(passBuffer,PASSWORD)==0) { // Success
                LCD_Clear(); LCD_String("Access Granted!"); 
                sprintf(uart_msg_buffer, "PASS: ACCESS_GRANTED\r\n");
                Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
                _delay_ms(1500);
                
                // Go to Main Menu
                menuIndex=0; topIndex=0; show_menu_display();
                currentState=STATE_MENU; reset_entry_flags();

                sprintf(uart_msg_buffer, "MENU: MAIN\r\n");
                Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
            } else { // Failure
                LCD_Clear(); LCD_String("Wrong Password!"); 
                sprintf(uart_msg_buffer, "PASS: ACCESS_DENIED\r\n");
                Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
                _delay_ms(1500);
                pass_entry_flag = 1; // Retry
            }
        } 
        else if(key=='*') { // CLEAR/BACK Key
            sprintf(uart_msg_buffer, "EVENT: CLEAR\r\n");
            Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
            index=0; passBuffer[0]='\0'; pass_entry_flag = 1; // Reset view
        }
    }
}

// ==========================================
// Main Menu Display & Logic
// ==========================================
void show_menu_display(void) {
    LCD_Clear();
    LCD_SetCursor(0,0);
    // Render first line with cursor
    if(menuIndex==topIndex) LCD_Char('>'); else LCD_Char(' '); 
    LCD_String(menuItems[topIndex]);

    // Render second line
    if(topIndex+1<TOTAL_OPTIONS) {
        LCD_SetCursor(1,0);
        if(menuIndex==topIndex+1) LCD_Char('>'); else LCD_Char(' ');
        LCD_String(menuItems[topIndex+1]);
    }
}

// Handles Navigation inside Main Menu
void handle_menu(void) {
    uint8_t btn = read_button();
    
    if(btn==1){  // UP
        sprintf(uart_msg_buffer, "EVENT: UP_BTN\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        if(menuIndex>0){ menuIndex--; if(menuIndex<topIndex) topIndex=menuIndex; show_menu_display(); _delay_ms(200);} 
    }
    else if(btn==2){ // DOWN
        sprintf(uart_msg_buffer, "EVENT: DOWN_BTN\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        if(menuIndex<TOTAL_OPTIONS-1){ menuIndex++; if(menuIndex>topIndex+1) topIndex=menuIndex-1; show_menu_display(); _delay_ms(200);} 
    }
    else if(btn==3){ // SELECT -> Switch to Specific Feature State
        LCD_Clear(); serialIdx=0; serialBuffer[0]='\0'; commandReady=0;
        sprintf(uart_msg_buffer, "EVENT: SELECT_OPT_%d\r\n", menuIndex+1);
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        
        reset_entry_flags();
        
        if(menuIndex==0) currentState=STATE_MONITOR;
        else if(menuIndex==1) currentState=STATE_VOICE_AC;
        else if(menuIndex==2) currentState=STATE_SET_TEMP;
        else if(menuIndex==3) currentState=STATE_SET_TEMP_SENSOR;
        
        _delay_ms(500);
    }
}

// ==========================================
// Monitor System State
// Displays Sensor Values: Temp (LM35), Gas (MQ2), Light (LDR)
// ==========================================
void handle_monitor(void) { 
    const unsigned long UPDATE_INTERVAL = 3000;  // Update sensors every 3 seconds
    static unsigned long last_update_time = 0;

    // 1. Log entry
    if(mon_entry_flag) { 
        sprintf(uart_msg_buffer, "STATE: MONITOR\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        mon_entry_flag = 0; 
    }
    
    // 2. Non-blocking sensor update
    if ((Timer_GetMillis() - last_update_time) >= UPDATE_INTERVAL) {
        last_update_time = Timer_GetMillis();
        
        // Read Sensors
        float temp = LM35_Read();
        uint16_t gas = MQ2_Read();
        uint16_t light = LDR_Read();

        // Format float temp (as printf %f support might be limited)
        int temp_int = (int)temp;
        int temp_dec = (int)((temp - temp_int) * 10);

        // Update LCD
        LCD_SetCursor(0, 0);
        snprintf(lcdBuffer, sizeof(lcdBuffer), "T:%d.%dC        ", temp_int, temp_dec); 
        LCD_String(lcdBuffer);

        LCD_SetCursor(1, 0);
        snprintf(lcdBuffer, sizeof(lcdBuffer), "G:%d L:%d      ", gas, light);
        LCD_String(lcdBuffer);

        // Send data to UART
        sprintf(uart_msg_buffer, "MON: T=%d.%d G=%d L=%d\r\n", temp_int, temp_dec, gas, light);
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
    }

    // 3. Check for Back Button (Button 4)
    if (read_button() == 4) { 
        sprintf(uart_msg_buffer, "EVENT: BACK_BTN\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        
        // Prevents buffer overflow on receiver if multiple messages sent too fast
        _delay_ms(50); 
        // ------------------------------------------------
        
        show_menu_display();
        currentState = STATE_MENU;
        
        sprintf(uart_msg_buffer, "MENU: MAIN\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        
        _delay_ms(200);
    }
}

// ==========================================
// Voice Control AC State
// Controls Motor via UART strings (simulating voice recognition)
// ==========================================
void handle_voice_ac(void) {
    static int current_val = 0;      // Current speed percentage
    static uint8_t need_lcd_update = 1; 

    if(voice_entry_flag) { 
        sprintf(uart_msg_buffer, "STATE: VOICE_AC\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        voice_entry_flag = 0; 
    }

    // 1. Process UART Commands
    if (commandReady) { 
        // Command: "reverse"
        if (strcmp(serialBuffer, "reverse") == 0) {
            motor_backward(255);           
            LCD_SetCursor(0, 0); LCD_String("Motor Backward  ");
            LCD_SetCursor(1, 0); LCD_String("Voice Mode AC  ");
            
            sprintf(uart_msg_buffer, "AC_ACT: REVERSE\r\n");
            Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
            need_lcd_update = 0; // Skip standard update to keep "Backward" msg         
        }
        // Command: "AC_XX" (e.g., AC_50)
        else if (strncmp(serialBuffer, "AC_", 3) == 0) {
            int val = atoi(&serialBuffer[3]); // Parse number after "AC_"
            // Validate allowed speeds
            if (val == 0 || val == 25 || val == 50 || val == 75 || val == 100) {
                current_val = val; 
                uint8_t pwm_val = (val == 0) ? 0 : map(val, 0, 100, 0, 255);
                motor_forward(pwm_val);
                
                sprintf(uart_msg_buffer, "AC_SET: %d\r\n", val);
                Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
                need_lcd_update = 1; 
            } else {
                 sprintf(uart_msg_buffer, "AC_ERR: INVALID\r\n");
                 Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
            }
        }
        serialBuffer[0] = '\0'; commandReady = 0;
    }

    // 2. Update LCD
    if (need_lcd_update) {
        LCD_SetCursor(0, 0); LCD_String("Voice Mode AC   ");
        LCD_SetCursor(1, 0); snprintf(lcdBuffer, 16, "Speed: %d %%      ", current_val);
        LCD_String(lcdBuffer); need_lcd_update = 0; 
    }

    // 3. Exit Logic
    if (read_button() == 4) { 
        sprintf(uart_msg_buffer, "EVENT: BACK_BTN\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        _delay_ms(50); 
        
        motor_stop(); show_menu_display(); currentState = STATE_MENU;
        sprintf(uart_msg_buffer, "MENU: MAIN\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        _delay_ms(200);
    }
}

// ==========================================
// Keypad Manual AC State
// User types 0-100 on keypad to set speed
// ==========================================
void handle_set_temp(void) {
    static char keyBuffer[5]; static uint8_t keyIndex = 0;
    static uint8_t update_display = 1;

    if(keypad_entry_flag) { 
        sprintf(uart_msg_buffer, "STATE: KEYPAD_AC\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        keypad_entry_flag = 0; keyIndex = 0; keyBuffer[0] = '\0'; update_display = 1;
    }

    // 1. Draw UI
    if (update_display) {
        LCD_SetCursor(0, 0); LCD_String("Enter AC Speed: ");
        LCD_SetCursor(1, 0);
        if (keyIndex == 0) LCD_String("_             ");
        else { snprintf(lcdBuffer, 16, "%s_             ", keyBuffer); LCD_String(lcdBuffer); }
        update_display = 0;
    }

    // 2. Handle Keypad Input
    char key = KEYPAD_GetKey(); 
    if (key) {
        sprintf(uart_msg_buffer, "KEY: %c\r\n", key);
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));

        if (key >= '0' && key <= '9') {
            if (keyIndex < 3) { keyBuffer[keyIndex++] = key; keyBuffer[keyIndex] = '\0'; update_display = 1; }
        } 
        else if (key == '*') { // STAR Key = REVERSE
            motor_backward(255); LCD_Clear(); LCD_String("Motor Backward");
            sprintf(uart_msg_buffer, "KEYPAD_ACT: REVERSE\r\n");
            Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
            _delay_ms(1500); keyIndex = 0; keyBuffer[0] = '\0'; update_display = 1;
        } 
        else if (key == '#') { // POUND Key = ENTER
            int val = atoi(keyBuffer);
            if (val > 100) { 
                val = 100;
                sprintf(uart_msg_buffer, "KEYPAD_ERR: LIMIT\r\n");
                Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
            } else if (val < 0) val = 0;

            // Map 1-100 range to 100-255 PWM (ensures motor starts spinning)
            uint8_t pwm_val = (val == 0) ? 0 : map(val, 1, 100, 100, 255);
            motor_forward(pwm_val);

            LCD_Clear(); snprintf(lcdBuffer, 16, "Set Speed: %d%%", val); LCD_String(lcdBuffer);
            sprintf(uart_msg_buffer, "KEYPAD_SET: %d\r\n", val);
            Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
            _delay_ms(1500); keyIndex = 0; keyBuffer[0] = '\0'; update_display = 1;
        }
    }

    // 3. Exit Logic
    if (read_button() == 4) { 
        sprintf(uart_msg_buffer, "EVENT: BACK_BTN\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        _delay_ms(50); 
        motor_stop(); show_menu_display(); currentState = STATE_MENU;
        sprintf(uart_msg_buffer, "MENU: MAIN\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        _delay_ms(200);
    }
}

// ==========================================
// Automatic AC Temperature Control State
// Adjusts PWM based on Set Temp vs Measured Temp
// ==========================================
void handle_set_temp_sensor(void) {
    static char keyBuffer[5]; static uint8_t keyIndex = 0;
    static uint8_t update_display = 1; static int set_temp = 25; 
    static unsigned long last_update_time = 0;
    const unsigned long UPDATE_INTERVAL = 3000; 
    static uint8_t wait_flag = 0;
    
    // Initial delay/splash
    if (!wait_flag) { LCD_Clear(); LCD_String("Waiting..."); _delay_ms(1000); wait_flag = 1; LCD_Clear(); }
    if(auto_entry_flag) { 
        sprintf(uart_msg_buffer, "STATE: AUTO_AC\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        auto_entry_flag = 0; 
    }
    
    // 1. Check for UART Mock Commands (For Testing)
    if (commandReady) { 
        if (strncmp(serialBuffer, "SET_MOCK=", 9) == 0) {
            mock_temp_val = atof(&serialBuffer[9]); use_mock_temp = 1;
            // Immediate update based on mock value
            float diff = mock_temp_val - set_temp;
            uint8_t pwm_val = (diff <= 0) ? 0 : (diff <= 1.0 ? 50 : (diff <= 3.0 ? 100 : (diff <= 5.0 ? 180 : 255)));
            sprintf(uart_msg_buffer, "PWM:%d\r\n", pwm_val);
            Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
            last_update_time = 0; // Force immediate sensor loop update
        } else if (strcmp(serialBuffer, "RESET_MOCK") == 0) {
            use_mock_temp = 0;
            sprintf(uart_msg_buffer, "MOCK_RESET\r\n");
            Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        }
        serialBuffer[0] = '\0'; commandReady = 0;
    }

    // 2. Draw Keypad Input UI
    if (update_display) {
        LCD_SetCursor(0, 0); LCD_String("Set Temp:       "); 
        LCD_SetCursor(0, 10);
        if (keyIndex == 0) LCD_String(""); else { snprintf(lcdBuffer, 16, "%s", keyBuffer); LCD_String(lcdBuffer); }
        LCD_SetCursor(1, 0); LCD_String("                "); update_display = 0;
    }

    // 3. Handle Keypad (Changing Set Temperature)
    char key = KEYPAD_GetKey();
    if (key) {
        sprintf(uart_msg_buffer, "KEY: %c\r\n", key);
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        if (key >= '0' && key <= '9') {
            if (keyIndex < 2) { keyBuffer[keyIndex++] = key; keyBuffer[keyIndex] = '\0'; update_display = 1; }
        } else if (key == '*') {  // CLEAR
            keyIndex = 0; keyBuffer[0] = '\0'; update_display = 1;
        } else if (key == '#') {  // ENTER
            if (keyIndex > 0) { 
                int input_val = atoi(keyBuffer);
                if (input_val == 99) {  // Hidden Feature: 99 triggers Reverse
                    motor_backward(255); LCD_Clear(); LCD_String("Motor Backward");
                    sprintf(uart_msg_buffer, "AUTO_ACT: REVERSE\r\n");
                    Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
                    _delay_ms(1500);
                } else {  // Normal Temperature Set
                    // Clamp temperature range
                    if (input_val < 18) input_val = 18; if (input_val > 30) input_val = 30;
                    set_temp = input_val;
                    LCD_Clear(); snprintf(lcdBuffer, 16, "Temp Set: %dC", set_temp); LCD_String(lcdBuffer);
                    sprintf(uart_msg_buffer, "AUTO_SET: %d\r\n", set_temp);
                    Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
                    _delay_ms(1500);
                }
                keyIndex = 0; keyBuffer[0] = '\0'; LCD_Clear(); update_display = 1; last_update_time = 0; 
            }
        }
    }

    // 4. Automatic Control Loop (Runs every 3 seconds if not typing)
    if (keyIndex == 0 && (Timer_GetMillis() - last_update_time) >= UPDATE_INTERVAL) {
        last_update_time = Timer_GetMillis();
        float current_temp = (use_mock_temp) ? mock_temp_val : LM35_Read();
        
        // Control Logic (Simple Proportional Step)
        float diff = current_temp - set_temp;
        uint8_t pwm_val = (diff <= 0) ? 0 : (diff <= 1.0 ? 50 : (diff <= 3.0 ? 100 : (diff <= 5.0 ? 180 : 255)));

        if (pwm_val == 0) motor_stop(); else motor_forward(pwm_val);

        // Update LCD with Status
        LCD_SetCursor(0, 0);
        int cur_int = (int)current_temp; int cur_frac = (int)((current_temp - cur_int) * 10);
        snprintf(lcdBuffer, 16, "Set:%dC Cur:%d.%dC", set_temp, cur_int, cur_frac); LCD_String(lcdBuffer);
        LCD_SetCursor(1, 0); snprintf(lcdBuffer, 16, "PWM:%d          ", pwm_val); LCD_String(lcdBuffer);
        
        // Log status if not mocking
        if(!use_mock_temp) {
             sprintf(uart_msg_buffer, "AUTO_MON: Set=%d Cur=%d.%d PWM=%d\r\n", set_temp, cur_int, cur_frac, pwm_val);
             Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        }
    }

    // 5. Exit Logic
    if (read_button() == 4) {
        sprintf(uart_msg_buffer, "EVENT: BACK_BTN\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        _delay_ms(50); 
        motor_stop(); show_menu_display(); currentState = STATE_MENU;
        sprintf(uart_msg_buffer, "MENU: MAIN\r\n");
        Uart_SendString(uart_msg_buffer, strlen(uart_msg_buffer));
        _delay_ms(200); wait_flag = 0; keyIndex = 0; keyBuffer[0] = '\0'; update_display = 1;
        reset_entry_flags();
    }
}

// Utility: Arduino-style map function implementation
long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}