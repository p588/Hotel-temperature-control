import time
import random  # Simulating sensor data for now
import paho.mqtt.client as mqtt

# MQTT settings
BROKER = 'mqtt.example.com'
PORT = 1883
TOPIC = 'hotel/branch/temperature'
CLIENT_ID = 'TemperaturePublisher'

def connect_mqtt():
    client = mqtt.Client(CLIENT_ID)
    client.connect(BROKER, PORT)
    return client

def publish(client):
    while True:
        temperature = random.uniform(20.0, 30.0)  # Simulated temperature data
        result = client.publish(TOPIC, temperature)
        
        # Wait for 60 seconds before sending the next data point
        time.sleep(60)

if __name__ == '__main__':
    client = connect_mqtt()
    client.loop_start()
    publish(client)
