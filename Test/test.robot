*** Settings ***
Documentation     Automotive Air Conditioning Control Panel with Authentication 
Library           SerialLibrary    encoding=ascii
Library           String
Suite Setup       Open Serial Connection
Suite Teardown    Close Serial Connection

*** Variables ***
${PORT}           COM11
${BAUDRATE}       9600
${TIMEOUT}        15 
${NEWLINE}        \r\n

*** Keywords ***
Open Serial Connection
    Add Port    ${PORT}    baudrate=${BAUDRATE}    timeout=${TIMEOUT}
    Open Port   ${PORT}
    Flush Port  ${PORT}
    Log To Console    SUCCESSFULLY CONNECTED TO ${PORT}.
    Log To Console    ACTION REQUIRED: Please perform a hardware reset on the device.
    Sleep    2s

Close Serial Connection
    Delete All Ports
    Log To Console    Serial connection closed.

Send Command
    [Arguments]    ${cmd}
    Flush Port    ${PORT}
    Write Data    ${cmd}${NEWLINE}
    Sleep    0.2s
    Log To Console    >> Command Sent: ${cmd}

Wait For UART
    [Arguments]    ${expected_substring}    ${loop_timeout}=20
    Log To Console    ... Waiting for response: "${expected_substring}" ...
    ${start_time}=    Get Time    epoch
    WHILE    True
        ${current_time}=    Get Time    epoch
        ${elapsed}=    Evaluate    ${current_time} - ${start_time}
        IF    ${elapsed} > ${loop_timeout}
            Fail    Timeout waiting for message: '${expected_substring}'
        END
        ${line}=    Read Until    ${NEWLINE}
        ${contains}=    Run Keyword And Return Status    Should Contain    ${line}    ${expected_substring}
        IF    ${contains}
            Log To Console    << Match Found: ${line}
            RETURN 
        END
    END

Prompt User
    [Arguments]    ${instruction}
    Log To Console    \n[USER ACTION]: ${instruction}

*** Test Cases ***

# A. System Initialization & Authentication

1 Verify System Initialization
    Wait For UART    SYS_READY

2 Verify Auth Menu Display
    Wait For UART    MENU: AUTH

3 Auth Menu - Navigate Down
    Prompt User    Please press the DOWN button.
    Wait For UART    EVENT: DOWN_BTN

4 Auth Menu - Navigate Up
    Prompt User    Please press the UP button.
    Wait For UART    EVENT: UP_BTN

5 Select Face Recognition Mode
    Prompt User    Please select the 'Face Detection' option.
    Wait For UART    EVENT: SELECT_BTN
    Wait For UART    STATE: FACE_WAIT

6 Verify Face Recognition Waiting State
    Wait For UART    FACE: WAITING

7 Simulate Successful Face Recognition
    Send Command     SYSTEM_INIT
    Wait For UART    FACE: ACCESS_GRANTED

8 Verify Main Menu Display
    Wait For UART    MENU: MAIN


# B. Menu Navigation

9 Main Menu - Navigate to Voice Control
    Prompt User    Press DOWN button to highlight 'Voice AC Ctrl'.
    Wait For UART    EVENT: DOWN_BTN

10 Main Menu - Navigate to Keypad Control
    Prompt User    Press DOWN button to highlight 'Keypad AC Set'.
    Wait For UART    EVENT: DOWN_BTN

11 Main Menu - Navigate to Set Temp
    Prompt User    Press DOWN button to highlight 'Set Temp AC'.
    Wait For UART    EVENT: DOWN_BTN

12 Main Menu - Wrap-Around Check
    Prompt User    Press DOWN button again to wrap to the top.
    Wait For UART    EVENT: DOWN_BTN

13 Main Menu - Navigate Up
    Prompt User    Press UP button.
    Wait For UART    EVENT: UP_BTN

14 Enter Sensor Monitor Mode
    Prompt User    Press SELECT on 'Monitor Sys'.
    Wait For UART    EVENT: SELECT_OPT_1
    Wait For UART    STATE: MONITOR

# C. Sensor Monitor Mode

15 Verify Temperature Reading
    Wait For UART    MON:

16 Verify Gas and Light Readings
    Wait For UART    G=

17 Exit Sensor Monitor Mode
    Prompt User    Please press the BACK button.
    Wait For UART    EVENT: BACK_BTN
    Wait For UART    MENU: MAIN

# D. Voice AC Control Mode

18 Enter Voice AC Mode
    Prompt User    Navigate to 'Voice AC Ctrl' (Option 2) and press SELECT.
    Wait For UART    EVENT: SELECT_OPT_2
    Wait For UART    STATE: VOICE_AC

19 Voice Command: AC OFF
    Send Command     AC_0
    Wait For UART    AC_SET: 0

20 Voice Command: AC Low Speed
    Sleep    0.5s
    Send Command     AC_25
    Wait For UART    AC_SET: 25

21 Voice Command: AC Medium Speed
    Sleep    0.5s
    Send Command     AC_50
    Wait For UART    AC_SET: 50

22 Voice Command: AC High Speed
    Sleep    0.5s
    Send Command     AC_75
    Wait For UART    AC_SET: 75

23 Voice Command: AC Max Speed
    Sleep    0.5s
    Send Command     AC_100
    Wait For UART    AC_SET: 100

24 Voice Command: Reverse Motor
    Sleep    0.5s
    Send Command     reverse
    Wait For UART    AC_ACT: REVERSE

25 Voice Command: Invalid Input Handling
    Sleep    0.5s
    Send Command     AC_999
    Wait For UART    AC_ERR: INVALID

26 Exit Voice AC Mode
    Prompt User    Please press the BACK button.
    Wait For UART    EVENT: BACK_BTN
    Wait For UART    MENU: MAIN

# E. Keypad AC Speed Mode

27 Enter Keypad AC Mode
    Prompt User    Navigate to 'Keypad AC Set' (Option 3) and press SELECT.
    Wait For UART    EVENT: SELECT_OPT_3
    Wait For UART    STATE: KEYPAD_AC

28 Keypad Input: Digit 5
    Prompt User    Press '5' on the keypad.
    Wait For UART    KEY: 5

29 Keypad Input: Digit 0
    Prompt User    Press '0' on the keypad.
    Wait For UART    KEY: 0

30 Keypad Input: Confirm Entry
    Prompt User    Press '#' to confirm.
    Wait For UART    KEYPAD_SET: 50

31 Keypad Input: Set Zero Speed
    Sleep    1.5s
    Prompt User    Press '0' then '#' on the keypad.
    Wait For UART    KEYPAD_SET: 0

32 Keypad Input: Reverse Motor
    Sleep    1.5s
    Prompt User    Press '*' on the keypad.
    Wait For UART    KEYPAD_ACT: REVERSE

33 Keypad Input: Over-Limit Handling
    Sleep    1.5s
    Prompt User    Type '1', '0', '5', then press '#'.
    Wait For UART    KEYPAD_ERR: LIMIT

34 Exit Keypad AC Mode
    Prompt User    Please press the BACK button.
    Wait For UART    EVENT: BACK_BTN
    Wait For UART    MENU: MAIN

# F. Auto-Temperature AC Mode

35 Enter Auto Temp Mode
    Prompt User    Navigate to 'Set Temp AC' (Option 4) and press SELECT.
    Wait For UART    EVENT: SELECT_OPT_4
    Wait For UART    STATE: AUTO_AC

36 Set Target Temperature
    Prompt User    Type '25' then press '#'.
    Wait For UART    AUTO_SET: 25

37 Auto Control: Below Target (Fan OFF)
    Sleep    2s 
    Send Command     SET_MOCK=20
    Wait For UART    PWM:0

38 Auto Control: Slightly Above Target (Fan Low)
    Sleep    0.5s
    Send Command     SET_MOCK=26
    Wait For UART    PWM:50

39 Auto Control: Moderately Above Target (Fan Med)
    Sleep    0.5s
    Send Command     SET_MOCK=28
    Wait For UART    PWM:100

40 Auto Control: High Above Target (Fan Max)
    Sleep    0.5s
    Send Command     SET_MOCK=35
    Wait For UART    PWM:255

41 Reset Mock Sensor Data
    Send Command     RESET_MOCK
    Wait For UART    MOCK_RESET

42 Exit Auto Temp Mode
    Prompt User    Please press the BACK button.
    Wait For UART    EVENT: BACK_BTN
    Wait For UART    MENU: MAIN

# G. Password Authentication

43 System Reset for Password Test
    Prompt User    Please perform a hardware reset on the device.
    Wait For UART    SYS_READY

44 Navigate to Password Option
    Prompt User    Press DOWN to highlight 'Password'.
    Wait For UART    EVENT: DOWN_BTN

45 Enter Password Screen
    Prompt User    Press SELECT to enter.
    Wait For UART    EVENT: SELECT_BTN
    Wait For UART    STATE: PASSWORD_ENTRY

46 Verify Incorrect Password
    Prompt User    Enter '1234' then press '#'.
    Wait For UART    PASS: ACCESS_DENIED

47 Verify Correct Password
    Sleep    2s
    Prompt User    Enter '1111' then press '#'.
    Wait For UART    PASS: ACCESS_GRANTED
    Wait For UART    MENU: MAIN