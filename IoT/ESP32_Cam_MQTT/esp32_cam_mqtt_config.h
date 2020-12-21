//WiFi Settings  -> credentials in wifi_credentials.h
#define WIFI_HOSTNAME "esp32-plantcam"

// MQTT Settings -> credentials in wifi_credentials.h
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883

/*
#define MQTT_TOPIC_LED_CMD "irri359/cam/cmd/LED"  // in-built red LED: send "on" / "off"
#define MQTT_TOPIC_LED "irri359/cam/LED"  // confirmation
#define MQTT_TOPIC_FLASH_CMD "irri359/cam/cmd/FLASH" // in-build white LED flashlight: send "on" / "off"
#define MQTT_TOPIC_FLASH "irri359/cam/FLASH"  // confirmation
#define MQTT_TOPIC_CMD_TAKEPHOTO "irri359/cam/cmd/takepicture" // request take a picture
#define MQTT_TOPIC_STATUS "irri359/cam/sensor/status"  // will send "CLICK" after sending picture - sued for troubleshooting
#define MQTT_TOPIC_NEWPICTURE "irri359/cam/sensor/newpicture"  // receive the picture
#define MQTT_TOPIC_CMD_SETCONFIG "irri359/cam/cmd/setconfig"  // send camera config
#define MQTT_TOPIC_CMD_SETBUFFERSIZE "irri359/cam/cmd/buffer"
*/

#define MQTT_TOPIC_LED_CMD "erfindergarten/cam/cmd/LED"  // in-built red LED: send "on" / "off"
#define MQTT_TOPIC_LED "erfindergarten/cam/LED"  // confirmation
#define MQTT_TOPIC_FLASH_CMD "erfindergarten/cam/cmd/FLASH" // in-build white LED flashlight: send "on" / "off"
#define MQTT_TOPIC_FLASH "erfindergarten/cam/FLASH"  // confirmation
#define MQTT_TOPIC_CMD_TAKEPHOTO "erfindergarten/cam/cmd/takepicture" // request take a picture
#define MQTT_TOPIC_STATUS "erfindergarten/cam/sensor/status"  // will send "CLICK" after sending picture - sued for troubleshooting
#define MQTT_TOPIC_NEWPICTURE "erfindergarten/cam/sensor/newpicture"  // receive the picture
#define MQTT_TOPIC_CMD_SETCONFIG "erfindergarten/cam/cmd/setconfig"  // send camera config
#define MQTT_TOPIC_CMD_SETBUFFERSIZE "erfindergarten/cam/cmd/buffer"

// Pin headers 
// - set for AI Thinker board
//TODO: allow switching other board definitions based on ESP32-CAM example 

#define FLASHLIGHT 4
#define LED 33

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
