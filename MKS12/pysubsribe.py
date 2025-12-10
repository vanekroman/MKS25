import base64
import json

import paho.mqtt.client as mqtt

# NOTE: Replace 'XX' with your actual PC number!
PC_NUMBER = "81"
LORA_TOPIC = f"v3/mpc-mks@ttn/devices/devkit-{PC_NUMBER}/up"

# --- MQTT Callbacks ---


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, reason_code, properties):
    """Called when the client connects to the MQTT broker."""
    print(f"✅ Connected with result code {reason_code}")
    # Subscribe to the specific LoRaWAN uplink topic
    client.subscribe(LORA_TOPIC)
    print(f"➡️ Subscribed to topic: {LORA_TOPIC}")


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    """Called when a message is received from the subscribed topic."""

    # Check if the message is from our expected LoRaWAN topic
    if msg.topic == LORA_TOPIC:
        try:
            # 1. Convert the byte payload to a regular string
            payload_str = msg.payload.decode("utf-8")

            # 2. Convert the JSON string into a Python dictionary
            pld = json.loads(payload_str)

            # 3. Extract and decode the device message (frm_payload is Base64)
            # Access the base64-encoded payload
            base64_payload = pld["uplink_message"]["frm_payload"]

            # Convert Base64 payload to a readable string
            decoded_message = base64.b64decode(base64_payload).decode()

            # 4. Extract required metadata
            # Time (from the overall message timestamp)
            time = pld["received_at"]

            # Frequency (from the first gateway's metadata)
            # LoRaWAN messages pass through a gateway; we can check the first gateway's metadata
            try:
                frequency = pld["uplink_message"]["rx_metadata"][0]["frequency"]
            except (KeyError, IndexError):
                frequency = "N/A (Frequency data not found)"

            # 5. Print the required information
            print("\n--- LoRaWAN Message Received ---")
            print(f"🕒 **Time:** {time}")
            print(f"📡 **Frequency:** {frequency} Hz")
            print(f"📜 **Decoded Message:** {decoded_message}")
            print("--------------------------------\n")

        except Exception as e:
            print(f"❌ Error processing message: {e}")
            print(f"Original Topic: {msg.topic}")
            # print(f"Original Payload: {msg.payload}") # Uncomment for debugging raw payload

    # Handle the $SYS/# topic if you keep the original subscription (though it's usually not needed)
    else:
        # The $SYS/# messages are typically used for broker status updates
        # We can ignore these for the primary task
        pass


# --- MQTT Client Setup and Connection ---

# Client ID is optional, but using VERSION2 is recommended.
mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

# Assign callback functions
mqttc.on_connect = on_connect
mqttc.on_message = on_message

# Set username and password for The Things Network (TTN)
# TTN Username: Application ID (mpc-mks@ttn)
# TTN Password: API Key
mqttc.username_pw_set("mpc-mks@ttn", "NNSXS.B75L4YCIWKRIXUGEDS2NNGXTNAV4PHW7POSNS2Y.EE42KQNJKP36YCSLRH5SIU4I5XXC2KOQGJPIT3PRK4PKNMPDNIVQ")

# Connect to the TTN MQTT broker
# Address: eu1.cloud.thethings.network, Port: 1883 (standard MQTT)
print("Connecting to MQTT broker...")
mqttc.connect("eu1.cloud.thethings.network", 1883, 60)

# Blocking call that processes network traffic and dispatches callbacks.
# This keeps the script running and listening for messages.
mqttc.loop_forever()
