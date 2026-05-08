import speech_recognition as sr
import serial
import time
import re  

def control_ac_system():
    if 'arduino' not in globals() or arduino is None:
        print(" Error: Arduino connection lost. Run Face ID first.")
        return

    print("\n Voice System Active (Connected to Existing Session)")
    recognizer = sr.Recognizer()
    recognizer.energy_threshold = 3000

    print(" Listening for AC commands... (e.g., 'Set AC to 25')")
    with sr.Microphone() as source:
        recognizer.adjust_for_ambient_noise(source)

        while True:
            try:
                print("Waiting for command...")
     
                source.stream.read(source.CHUNK)
                audio = recognizer.listen(source, timeout=5, phrase_time_limit=5)
                text = recognizer.recognize_google(audio).lower()

                print(f" You said: '{text}'")


                # ----- Hi COMMAND -----
                if "ready" in text or "Hi" in text:
                    print(" System Ready")
                    continue

                # ----- EXIT / THANK YOU -----
                if "thank" in text or "thanks" in text:
                    print(" You're welcome! Exiting Voice Mode.")
                    break

                # ----- REVERSE COMMAND -----
                if "reverse" in text or "turn back" in text or "backwards" in text:
                    print(" Sending Command: REVERSE")
                    if arduino:
                        arduino.write(b"reverse\n")
                    continue

                # ----- AC  COMMANDS -----
                if "ac" in text or "air condition" in text or "temperature" in text:

                    found_numbers = re.findall(r'\d+', text)
                    if found_numbers:
                        temp_value = int(found_numbers[0])
                        valid_temps = [0, 25, 50, 75, 100]
                        if temp_value in valid_temps:
                            print(f" Setting AC to {temp_value}%")

                            if arduino:
                                command_str = f"AC_{temp_value}\n"
                                arduino.write(command_str.encode())
                        else:
                            print(f" {temp_value} is not valid. Use: 0, 25, 50, 75, 100.")

                    else:
                        print(" No temperature number heard.")

            except sr.WaitTimeoutError:
                pass  
            except sr.UnknownValueError:
                pass  
            except Exception as e:
                print(f"Error: {e}")


control_ac_system()