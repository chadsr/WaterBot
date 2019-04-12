#ifndef MAIN_H
#define MAIN_H

#define API_PORT 1337
#define WIFI_SSID "somewifi"
#define WIFI_PASSWORD "somepassword"
#define JSON_BUFF_SIZE 1024

#define PIN_PUMP 17
#define PIN_DHT 16
#define DHT_TYPE DHT21

#define NUM_MOISTURE_SENSORS 2
const int PINS_MOISTURE[NUM_MOISTURE_SENSORS]= {34, 35};
#define PIN_MOISTURE_VCC 5 // Pin to control power to the moisture sensors (via transistor). Limits corrosion this way.

#define MAX_MOISTURE_VALUE 4095 // Change this to values your sensor reads
#define MIN_MOISTURE_VALUE 0 // Change this to values your sensor reads
#define LOW_MOISTURE_TRIGGER 60 // Low moisture level in percentage, which will trigger the pumping process
#define TARGET_MOISTURE 80 // Target moisture level in percentage
#define PUMP_SECONDS 10 // Number of seconds to pump for before waiting for another reading
#define WAIT_SECONDS 20 // Seconds to wait inbetween pumps, before checking the moisture level again

#define OLED_RESET 16
#define OLED_WIDTH 128
#define OLED_HEIGHT 32
#define PIN_SDA 21
#define PIN_SCL 22

#define MOISTURE_MSG "Soil Moisture:"
#define AIR_TEMP_MSG "Air Temp:"
#define HUMIDITY_MSG "Air Humidity:"

#endif