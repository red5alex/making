/*

   MQTT Servo Unit

      Servo default GPIO 15 (or adapt variable SERVO_A_PIN).
      This allows to attach the 3 wires of a servo directly to the +5V pin of ESP32.

      This sketch uses a servo to turn a ball valve open/close when being called
      on the subscribed MQTT.
*/
#include "esp32_mqtt_servo_relais_PCA9685_config.h"

#define FIRMWARE_NAME "Irri359-StregerEmulation"
#define FIRMWARE_SOURCE "esp32_mqtt_servo_relais_PCA9685.ino"
#define FIRMWARE_VERSION "0.002"
#define FIRMWARE_DATE "2021-05-23"

#include <Arduino.h>
#define LED_BUILTIN 2

#include <ArduinoJson.h>

//everything for the PCA9685 driver
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// load servo library
#include "are_servo.h"

// load GPIO
#include "are_relais_config.h"

// load wifi library and - settings
#include "are_wifi.h"
#include "are_wifi_config.h"
const char* wifi_ssid = WIFI_SSID;
const char* wifi_password = WIFI_PASSWORD;
const char* wifi_hostname = WIFI_HOSTNAME;

//MQTT settings
#include <PubSubClient.h>
#include "are_mqtt.h"
#include "are_mqtt_config.h"
const char* mqtt_server = MQTT_SERVER;
WiFiClient espClient;
PubSubClient mqttclient(espClient);
long lastMsg = 0;
long mqtt_update_interval = MQTT_UPDATE_INTERVAL;


// For JSON deserialization
StaticJsonDocument<200> doc;


/* water pump and flow meter emulator
  / Note: Because the setup has no actual water meter, this is emulated using the constant flow rate of the peristaltic pump
*/

// Telemetry variables - these will be send regulary
float pump_totalizer = 0.;
float pump_target = 1000.;
//float pump_voltage;

// status variables - these are for internal use
bool pump_status_on;
//bool pump_connected = false;
//bool pump_connected_demo = DEMO_MODE;
//float pump_flow_rate = FLOW_RATE; // ml per ms

float pump_time_increment_t0 = 0.;
float pump_counter = 0.;
float pump_counter_autostop = 0.;




// servo settings (obsolete?)
int servoA_closed_angle = SERVO_A_CLOSE;
int servoA_open_angle = SERVO_A_OPEN;

//MQTT callback
void callback(char* topic, byte* message, unsigned int length) {

  Serial.println(length);

  Serial.print("mqqt message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // update state variables


  if (String(topic) == "red5alex/valve01/command/generic") {
    Serial.print("mqtt-client: received generic command ");
    Serial.print(messageTemp);

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, messageTemp);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      mqttclient.publish("red5alex/valve01/error", "ESP32: deserializeJson() failed");
      return;
    }

    Serial.println();
    const char* command = doc["command"];
    Serial.println(command);

    if (String(command) == "setPWM")
    {
      int channel = doc["channel"];
      int start_tick = doc["start_tick"];
      int stop_tick = doc["stop_tick"];

      Serial.print("Setting PWM on channel "); Serial.print(channel); Serial.print(": "); Serial.print(start_tick); Serial.print(".."); Serial.print(stop_tick);
      pwm.setPWM(channel, start_tick, stop_tick);
      mqttclient.publish("red5alex/valve01/status/confirm", doc["confirm_as"]);
      Serial.println(" - done!");
      // pulselength = map(degrees, 0, 180, SERVOMIN, SERVOMAX);
    }
    else if (String(command) == "set-pump_totalizer") {
      pump_totalizer = doc["value"];
      mqttclient.publish("red5alex/valve01/status/confirm", "set_totalizer");
      Serial.print("pump_totalizer set to "); Serial.println(pump_totalizer);
    }
    else if (String(command) == "set-pump_target") {
      pump_target = doc["value"];
      mqttclient.publish("red5alex/valve01/status/confirm", "set_target");
      Serial.print("pump_target set to "); Serial.println(pump_target);
    }
    else if (String(command) == "set-mqtt_update_interval") {
      mqtt_update_interval = doc["value"];
      mqttclient.publish("red5alex/valve01/status/confirm", "set_updateintervall");
      Serial.print("mqtt_update_interval set to "); Serial.println(mqtt_update_interval);
    }
    else
    {
      Serial.print("don't know what to do with '"); Serial.print(String(command)); Serial.print("'");  
      mqttclient.publish("red5alex/valve01/error", "ESP32: Don't know what to do with this command");
    }
  }


  // Changes the output state according to the message and publish a confirmation
  if (String(topic) == "red5alex/valve01/command/pw_servoA") {

    Serial.print("mqtt-client: Setting pulse width to ");
    Serial.print(messageTemp);
    int target_pw = messageTemp.toInt();

    servo_A.write(target_pw);

    mqttclient.publish("red5alex/valve01/status/angle_servoA", "set");
  }



  // Changes the output state according to the message and publish a confirmation
  if (String(topic) == "red5alex/valve01/command/angle_servoA") {

    Serial.print("mqtt-client: Setting angle to ");
    Serial.print(messageTemp);
    float target_angle = messageTemp.toInt();
    servo_set_angle_instant(&servo_A, target_angle);

    char tempString[8];
    dtostrf(target_angle, 1, 2, tempString);
    mqttclient.publish("red5alex/valve01/status/angle_servoA", tempString);
  }

  // Changes the output state according to the message and publish a confirmation
  if (String(topic) == "red5alex/valve01/command/valveA") {
    if (messageTemp == "open") {
      Serial.println("opening valveA");
      digitalWrite(LED_BUILTIN, HIGH);
      servo_A_set_angle(-90);
      mqttclient.publish("red5alex/valve01/status/valveA", "open");
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if (messageTemp == "close") {
      Serial.println("closing valveA");
      digitalWrite(LED_BUILTIN, HIGH);
      servo_A_set_angle(90);
      mqttclient.publish("red5alex/valve01/status/valveA", "closed");
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  // Changes the output state according to the message and publish a confirmation
  if (String(topic) == "red5alex/valve01/command/valveB") {
    if (messageTemp == "open") {
      Serial.println("opening valveB");
      digitalWrite(LED_BUILTIN, HIGH);
      servo_A_set_angle(-90);
      mqttclient.publish("red5alex/valve01/status/valveB", "open");
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if (messageTemp == "close") {
      Serial.println("closing valveB");
      digitalWrite(LED_BUILTIN, HIGH);
      servo_A_set_angle(90);
      mqttclient.publish("red5alex/valve01/status/valveB", "closed");
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  // Relais 0
  if (String(topic) == "red5alex/valve01/command/relais00") {
    if (messageTemp == "open") {
      Serial.println("opening valveA");
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(RELAIS_0_PIN, LOW);
      mqttclient.publish("red5alex/valve01/status/relais00", "open");
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if (messageTemp == "close") {
      Serial.println("closing valveA");
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(RELAIS_0_PIN, HIGH);
      mqttclient.publish("red5alex/valve01/status/relais00", "closed");
      digitalWrite(LED_BUILTIN, LOW);
    }
  }


  // Relais 1
  if (String(topic) == "red5alex/valve01/command/relais01") {
    if (messageTemp == "open") {
      Serial.println("opening valveA");
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(RELAIS_1_PIN, LOW);
      pump_status_on = true;
      mqttclient.publish("red5alex/valve01/status/relais01", "open");
      digitalWrite(LED_BUILTIN, LOW);

    }
    else if (messageTemp == "close") {
      Serial.println("closing valveA");
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(RELAIS_1_PIN, HIGH);
      pump_status_on = false;
      mqttclient.publish("red5alex/valve01/status/relais01", "closed");
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  // Relais 2
  if (String(topic) == "red5alex/valve01/command/relais02") {
    if (messageTemp == "open") {
      Serial.println("opening valveA");
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(RELAIS_2_PIN, LOW);
      mqttclient.publish("red5alex/valve01/status/relais02", "open");
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if (messageTemp == "close") {
      Serial.println("closing valveA");
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(RELAIS_2_PIN, HIGH);
      mqttclient.publish("red5alex/valve01/status/relais02", "closed");
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  // Relais 3
  if (String(topic) == "red5alex/valve01/command/relais03") {
    if (messageTemp == "open") {
      Serial.println("opening valveA");
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(RELAIS_3_PIN, LOW);
      mqttclient.publish("red5alex/valve01/status/relais03", "open");
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if (messageTemp == "close") {
      Serial.println("closing valveA");
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(RELAIS_3_PIN, HIGH);
      mqttclient.publish("red5alex/valve01/status/relais03", "closed");
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  // Relais 4
  if (String(topic) == "red5alex/valve01/command/relais04") {
    if (messageTemp == "open") {
      Serial.println("opening valveA");
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(RELAIS_4_PIN, LOW);
      mqttclient.publish("red5alex/valve01/status/relais04", "open");
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if (messageTemp == "close") {
      Serial.println("closing valveA");
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(RELAIS_4_PIN, HIGH);
      mqttclient.publish("red5alex/valve01/status/relais04", "closed");
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

}

void setup() {

  // setup PCA9685 PWM Driver
  Wire.begin(32, 33);  //SDA, SCL
  pwm.begin();
  pwm.setPWMFreq(50); // This is the maximum PWM frequency

  // configure Servo
  servo_A.setPeriodHertz(servo_A_dutycycle);
  servo_A.attach(servo_A_pin, servo_A_pulsewidth_min, servo_A_pulsewidth_max);
  servo_A_position = get_pw_of_angle(servo_A_angle);
  servo_A_set_angle(servo_A_angle);
  delay(500);

  // initialize Relais GPIO
  pinMode(RELAIS_1_PIN, OUTPUT);
  pinMode(RELAIS_2_PIN, OUTPUT);
  pinMode(RELAIS_3_PIN, OUTPUT);
  pinMode(RELAIS_4_PIN, OUTPUT);
  digitalWrite(RELAIS_1_PIN, HIGH);  // Set High - Low Level Trigger
  digitalWrite(RELAIS_2_PIN, HIGH);  // Set High - Low Level Trigger
  digitalWrite(RELAIS_3_PIN, HIGH);  // Set High - Low Level Trigger
  digitalWrite(RELAIS_4_PIN, HIGH);  // Set High - Low Level Trigger

  // SERVO CONTROLLER
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // initialize device
  Serial.begin(115200);
  Serial.println(FIRMWARE_NAME);
  Serial.println(FIRMWARE_SOURCE);
  Serial.println(FIRMWARE_VERSION);
  Serial.println(FIRMWARE_DATE);

  // initialize GPIO pins
  pinMode(LED_BUILTIN, OUTPUT);

  // connect to Wifi
  connect_to_wifi(wifi_ssid, wifi_password, wifi_hostname);

  // Connect to MQTT Server
  mqttclient.setServer(mqtt_server, 1883);
  mqttclient.setCallback(callback);

}

void loop() {

  // let mqtt work
  mqttclient.loop();

  // ensure connection to mqtt broker
  mqtt_check_connection(&mqttclient);

  // increment counter if pump is active and has power.
  // in demo mode the counter will continue counting even if no pump is connected.
  // increment is duration since last loop and flow rate (ml / ms)
  long now = millis();
  if (pump_status_on)
  {
    pump_totalizer = pump_totalizer + (now - pump_time_increment_t0) / 1000 * FLOW_RATE;
    pump_target = pump_target - (now - pump_time_increment_t0) / 1000 * FLOW_RATE;
  }

  pump_time_increment_t0 = now;

  if (pump_target < 0.)
  {
    Serial.println("closing Relais 1 - target reached!");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(RELAIS_1_PIN, HIGH);
    pump_status_on = false;
    mqttclient.publish("red5alex/valve01/status/relais01", "closed");
    digitalWrite(LED_BUILTIN, LOW);

    pump_target = 0.; 
    
  }

  // regularly publish all sensor readings and states via MQTT after a set intervall
  if (now - lastMsg > mqtt_update_interval) {

    // reset timer
    lastMsg = now;

    // publish emulated water counter value
    char tempString[8];
    dtostrf(pump_totalizer , 1, 2, tempString);
    Serial.print("  pump_totalizer: ");
    Serial.println(tempString);
    mqttclient.publish("red5alex/valve01/status/pump_totalizer", tempString);

    // publish emulated water target value
    tempString[8];
    dtostrf(pump_target , 1, 2, tempString);
    Serial.print("  pump_target: ");
    Serial.println(tempString);
    mqttclient.publish("red5alex/valve01/status/pump_target", tempString);


    // publish emulated water target value
    tempString[8];
    dtostrf(mqtt_update_interval , 1, 2, tempString);
    Serial.print("  mqtt_update_interval: ");
    Serial.println(tempString);
    mqttclient.publish("red5alex/valve01/status/mqtt_update_interval", tempString);
    
  }


}
