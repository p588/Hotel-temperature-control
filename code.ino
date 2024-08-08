#include <WiFi.h>
#include <PubSubClient.h>
#include <FS.h>
#include <SPIFFS.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* mqtt_server = "your_MQTT_BROKER_IP";
const char* topic = "hotel/room/temperature";

WiFiClient espClient;
PubSubClient client(espClient);

const int sensorPin = 34;  // Analog pin for temperature sensor
const int buzzerPin = 25;  // Buzzer pin
float threshold = 30.0;    // Temperature threshold
int consecutiveCount = 0;
const int requiredCount = 5; // 5 data points = 5 minutes
unsigned long previousMillis = 0; 
const long interval = 60000; // 60 seconds

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Clear buzzer alarm
  digitalWrite(buzzerPin, LOW);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read temperature data
    int analogValue = analogRead(sensorPin);
    float voltage = analogValue * (3.3 / 4095.0);  // ESP32 ADC is 12-bit
    float temperature = (voltage - 0.5) * 100.0;   // Assuming LM35 sensor

    // Save data point locally with timestamp
    saveDataLocally(temperature);

    // Publish temperature data
    char msg[50];
    snprintf(msg, 50, "Temperature: %2.2f", temperature);
    Serial.print("Publishing: ");
    Serial.println(msg);
    client.publish(topic, msg);

    // Check temperature against threshold
    if (temperature > threshold) {
      consecutiveCount++;
      if (consecutiveCount >= requiredCount) {
        digitalWrite(buzzerPin, HIGH);
        Serial.println("Alarm! Temperature threshold exceeded for 5 minutes.");
      }
    } else {
      consecutiveCount = 0;
      digitalWrite(buzzerPin, LOW);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void saveDataLocally(float temperature) {
  File file = SPIFFS.open("/data.txt", FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }

  // Get timestamp
  String timestamp = String(millis() / 1000);
  
  // Format: timestamp, temperature
  String data = timestamp + ", " + String(temperature) + "\n";
  
  // Write data to file
  if(file.print(data)){
    Serial.println("Data saved: " + data);
  } else {
    Serial.println("Failed to save data");
  }

  file.close();
}
