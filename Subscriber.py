import time
import paho.mqtt.client as mqtt
import os
from datetime import datetime

# MQTT settings
BROKER = 'mqtt.example.com'
PORT = 1883
TOPIC = 'hotel/branch/temperature'
CLIENT_ID = 'TemperatureSubscriber'

# Threshold and duration settings
THRESHOLD = 25.0  # Threshold temperature
ALARM_DURATION = 5  # 5 minutes

# File to store the data
LOG_FILE = 'temperature_log.txt'

# List to store recent readings
recent_readings = []


def connect_mqtt():
    client = mqtt.Client(CLIENT_ID)
    client.connect(BROKER, PORT)
    return client


def log_temperature(temperature):
    with open(LOG_FILE, 'a') as file:
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        file.write(f"{timestamp}: {temperature}\n")


def check_alarm():
    global recent_readings
    if len(recent_readings) == ALARM_DURATION:
        if all(temp > THRESHOLD for temp in recent_readings):
            print("ALARM! Temperature crossed the threshold for 5 minutes!")
            # Here you can add code to trigger a physical alarm or send a notification
        recent_readings.pop(0)


def on_message(client, userdata, message):
    temperature = float(message.payload.decode())
    log_temperature(temperature)
    recent_readings.append(temperature)
    check_alarm()


def subscribe(client):
    client.subscribe(TOPIC)
    client.on_message = on_message


if __name__ == '__main__':
    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()
