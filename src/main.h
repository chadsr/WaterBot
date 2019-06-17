#ifndef MAIN_H
#define MAIN_H

#define API_PORT 80
#define API_ENDPOINT "/readings"
#define WIFI_SSID "somewifi"
#define WIFI_PASSWORD "somepassword"
#define JSON_BUFF_SIZE 1024

#define PIN_PUMP 17
#define PIN_DHT 16
#define DHT_TYPE DHT21

#define NUM_MOISTURE_SENSORS 2
const int PINS_MOISTURE[NUM_MOISTURE_SENSORS]= {34, 35};
#define PIN_MOISTURE_VCC 5 // Pin to control power to the moisture sensors (via transistor). Limits corrosion this way.

#define MAX_MOISTURE_VALUE 760 // Change this to values your sensor reads (Value when moisture is maximum)
#define MIN_MOISTURE_VALUE 1700// Change this to values your sensor reads (Value when moisture is minimum)
#define LOW_MOISTURE_TRIGGER 30 // Low moisture level in percentage, which will trigger the pumping process
#define TARGET_MOISTURE 60 // Target moisture level in percentage
#define PUMP_SECONDS 20 // Number of seconds to pump for before waiting for another reading
#define WAIT_SECONDS 30 // Seconds to wait inbetween pumps, before checking the moisture level again. Also waiting time between checking sensors.

#define OLED_RESET 16
#define OLED_WIDTH 128
#define OLED_HEIGHT 32
#define PIN_SDA 21
#define PIN_SCL 22

#define MOISTURE_MSG "Soil Moisture:"
#define AIR_TEMP_MSG "Air Temp:"
#define HUMIDITY_MSG "Air Humidity:"

#endif