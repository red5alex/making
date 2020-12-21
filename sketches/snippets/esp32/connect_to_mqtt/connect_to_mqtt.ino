#include <Arduino.h>
#include <PubSubClient.h>

#include "credentials.h"

// load wifi library and - settings
#include "are_wifi_config.h"
#include "are_wifi.h"
#include "are_mqtt_config.h"
#include "are_mqtt.h"

// variables
int magnetic_flux_dens;  // a random measurement for testing
long lastMsg = 0; // time counter for MQTT intervalls

//MQTT settings
WiFiClient espClient;
PubSubClient mqttclient(espClient);

/* MQTT callback
 * This code sections reacts to incoming MQTT messages.
 */
 
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("mqqt message arrived on topic: ");
  Serial.println(topic);

  // cast payload to string object
  String messageTemp;
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Changes the output state according to the message and publish a confirmation
  if (String(topic) == TOPIC_LED_CMD) {
    if (messageTemp == "on") {
      Serial.println("LED on");
      digitalWrite(GPIO_LED, HIGH);
      mqttclient.publish(TOPIC_LED_STATUS, "on");
    }
    else if (messageTemp == "off") {
      Serial.println("LED off");
      digitalWrite(GPIO_LED, LOW);
      mqttclient.publish(TOPIC_LED_STATUS, "off");
    }

  // Add more commands here ...
  
  }
}

void setup() {

  // initialize device
  Serial.begin(115200);
  pinMode(GPIO_LED, OUTPUT);

  // connect to Wifi
  connect_to_wifi(WIFI_SSID, WIFI_PASSWORD, WIFI_HOSTNAME);

  // Connect to MQTT Server
  mqttclient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttclient.setCallback(callback);

}

void loop() {

  // let mqtt work
  mqttclient.loop();

  /* 
   * PUBLISH SENSOR READINGS
   * 
   * This section is called regularly (duration defined in MQTT_UPDATE_INTERVALL in milliseconds)
   * Use for regular MQTT messages, typically status and sensors.
   */
  
  long now = millis();
  if (now - lastMsg > MQTT_UPDATE_INTERVALL) {
    lastMsg = now;

    // ensure connection to mqtt broker
    mqtt_check_connection(&mqttclient);

    // get sensor readings
    magnetic_flux_dens = hallRead();

    // Convert sensor reading from float to a char array
    char tempString[8];  // increase array length if having runtime errors
    dtostrf(magnetic_flux_dens, 1, 2, tempString);
    Serial.print("Hall Sensor Output: ");
    Serial.print(tempString);
    mqttclient.publish(TOPIC_SENSOR_HALL, tempString);
  }

}
