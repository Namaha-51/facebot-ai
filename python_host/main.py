import os
import time
import serial
from google import genai
from google.genai import types

# Configure Serial Port (Adjust COM port if necessary)
SERIAL_PORT = "COM15"
BAUD_RATE = 115200

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)  # Wait for ESP32 reset
    print(f"[+] Connected to ESP32 on {SERIAL_PORT}")
except Exception as e:
    print(f"[-] Serial Connection Error: {e}")
    exit()

# Configure Gemini Client using google-genai SDK
client = genai.Client(api_key=os.environ.get("GEMINI_API_KEY"))

SYSTEM_INSTRUCTION = """
You are FaceBot, an expressive physical companion robot living on an ESP32 micro-controller desktop setup.
You communicate using brief sentences suitable for a 16x2 LCD display (MAXIMUM 32 CHARACTERS total per output).

Format every single response as:
EMOTION:TEXT

Valid EMOTION tags: HAPPY, ANGRY, SLEEP, SAD, SURPRISE, NEUTRAL.

Rules:
1. TEXT must strictly be under 32 characters.
2. Be witty, slightly theatrical, and expressive.
3. Example: HAPPY:I AM ONLINE HUMAN!
"""

chat = client.chats.create(
    model="gemini-2.5-flash",
    config=types.GenerateContentConfig(
        system_instruction=SYSTEM_INSTRUCTION,
        temperature=0.7,
    )
)

print("\n--- FaceBot Online (Ctrl+C to quit) ---\n")

while True:
    try:
        user_input = input("You: ")
        if not user_input.strip():
            continue

        response = chat.send_message(user_input)
        reply_text = response.text.strip()

        print(f"Robot Payload: {reply_text}")

        # Send formatted payload to ESP32 over Serial
        ser.write(f"{reply_text}\n".encode('utf-8'))

    except KeyboardInterrupt:
        print("\nExiting FaceBot Controller...")
        ser.close()
        break
    except Exception as e:
        print(f"Error: {e}")
