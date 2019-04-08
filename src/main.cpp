#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <string.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

#include "main.h"

struct SensorReadings {
  int moisture[NUM_MOISTURE_SENSORS];
  int avgMoisture;
  float humidity;
  float temperature;
};

TwoWire tw = TwoWire(1); // FOR SDA/SCL of OLED display
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &tw, OLED_RESET); // OLED DISPLAY
WebServer Server(API_PORT); // For JSON API
DHT dht; // For temp/humidity sensor

// The struct we will be keeping an up to date record of our readings in
struct SensorReadings sensorReadings = {};

// Returns a moisture value between 0 and 100, where 0 is dryest and 100 is fully submersed
void updateMoistureReadings() {
  // Read from the available moisture sensors and 
  for (int i = 0; i < NUM_MOISTURE_SENSORS; i++) {
    int adc = analogRead(PINS_MOISTURE[i]);
    // Map the analog value to a value between 0 and 100 for easier reading
    sensorReadings.moisture[i] = 100 - int(map(adc, MIN_MOISTURE_VALUE, MAX_MOISTURE_VALUE, 0, 100));
  }

  // Get the average value of all moisture sensors
  int avgMoisture = 0;
  for (int i = 0; i < NUM_MOISTURE_SENSORS; i++) {
    avgMoisture += sensorReadings.moisture[i];
  }
  sensorReadings.avgMoisture = (int) round(avgMoisture / NUM_MOISTURE_SENSORS);
}

void updateTempHumidity() {
  delay(dht.getMinimumSamplingPeriod());

  sensorReadings.humidity = dht.getHumidity();
  sensorReadings.temperature = dht.getTemperature();
}

void updateSensorReadings() {
  updateMoistureReadings();
  updateTempHumidity();
}

void pumpWater(int seconds) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.printf("Watering for %is...", seconds);
  display.setCursor(0, 10);
  display.printf("Then waiting %is...", WAIT_SECONDS);
  display.display();
  digitalWrite(PIN_PUMP, HIGH); // Trigger the relay
  delay(seconds * 1000); // Wait for the given number of seconds before closing the relay again
  digitalWrite(PIN_PUMP, LOW);
}

void displayReadings() {
  int spacing = int(OLED_HEIGHT / 3);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.printf("%s %i%%", MOISTURE_MSG, sensorReadings.avgMoisture);
  display.setCursor(0, spacing);
  display.printf("%s %.2f%C", AIR_TEMP_MSG, sensorReadings.temperature, (char)223);
  display.setCursor(0, spacing * 2);
  display.printf("%s %.2f%%", HUMIDITY_MSG, sensorReadings.humidity);
  display.setCursor(0, spacing * 3);  
  display.display();
}

void getReadingsJSON(char *dest, size_t size) {
  StaticJsonDocument<JSON_ARRAY_SIZE(NUM_MOISTURE_SENSORS) + JSON_OBJECT_SIZE(4)> json;
  JsonArray moistureValues = json["moisture_values"].as<JsonArray>();

  for (int i = 0; i < NUM_MOISTURE_SENSORS; i++) {
    moistureValues.add(sensorReadings.moisture[i]);
  }

  json["avg_moisture"].set(sensorReadings.avgMoisture);
  json["humidity"].set(sensorReadings.humidity);
  json["temperature"].set(sensorReadings.temperature);

  serializeJson(json, dest, size);
}

void handleNotFound() {
  Server.send(404, "text/plain", "Page Not Found");
}

void handleReadings() {
  char buffer[JSON_BUFF_SIZE];
  getReadingsJSON(buffer, JSON_BUFF_SIZE);
  Server.send(200, "text/json", String(buffer));
}

void setupServer() {
  Server.onNotFound(handleNotFound);
  Server.on("/readings", HTTP_GET, handleReadings);
  Server.begin();
}

void setupWireless() {
  // Prevent power-saving mode
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);
  WiFi.mode(WIFI_STA);

  // Attempt network connection on loop until success
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  bool inverter = true; // Value used to make a simple waiting animation
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Connecting to:");
    display.setCursor(0, 10);
    display.print(WIFI_SSID);
    display.setCursor(0, 20);

    if (inverter == true) {
      display.print("\\");
    } else {
      display.print("/");
    }

    display.display();
    inverter  = !inverter;
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Connected!");
  display.setCursor(0, 10);
  display.print("IP address:");
  display.setCursor(0, 20);
  char buffer[15]; // Buffer large enough to hold an IPv4 address as string
  WiFi.localIP().toString().toCharArray(buffer, 15);
  display.print(buffer);
  display.display();

  delay(5000); // Delay so the IP can be read
}

void setup() {
  Serial.begin(9600);
  dht.setup(PIN_DHT, DHT::DHT_TYPE);
  tw.begin(PIN_SDA, PIN_SCL);
  pinMode (PIN_PUMP, OUTPUT);
  delay(3000); // DM OLED has resistors R3 and R4 swapped, causing reset time of 2.7sec... Here's a hack for stock units
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);// initialize with the I2C addr 0x3C

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1.5);

  setupWireless();
}

void loop() {
  updateSensorReadings();
  Server.handleClient();
  displayReadings();

  if (sensorReadings.avgMoisture <= LOW_MOISTURE_TRIGGER) { // Trigger the pumping process
    while (sensorReadings.avgMoisture < TARGET_MOISTURE) {
      pumpWater(PUMP_SECONDS);
      delay(WAIT_SECONDS * 1000);
      updateSensorReadings(); // Read the moisture level again
      displayReadings();
    }
  }
}