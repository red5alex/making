#include "esp_camera.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> //ArduinoJSON6
DynamicJsonDocument CONFIG(2048);

#include "wifi_credentials.h"
#include "esp32_cam_mqtt_config.h"

const char* topic_BUFFERSIZE = MQTT_TOPIC_CMD_SETBUFFERSIZE;
const char* topic_PHOTO = MQTT_TOPIC_CMD_TAKEPHOTO;
const char* topic_CONFIG = MQTT_TOPIC_CMD_SETCONFIG;
const char* topic_UP = MQTT_TOPIC_NEWPICTURE;
const char* topic_STATUS = MQTT_TOPIC_STATUS;
const char* mqttUser = MQTT_USER;
const char* mqttPassword = MQTT_PASSWORD;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(String topic, byte* message, unsigned int length) {
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  if (topic == topic_PHOTO) {
    Serial.print("taking picture...");
    take_picture();
    Serial.println();
  }
  if (topic == topic_CONFIG) {
    deserializeJson(CONFIG, messageTemp);
    Serial.println(messageTemp);
    sensor_t * s = esp_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_VGA); //QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    s->set_vflip(s, CONFIG["vflip"]); //0 - 1
    s->set_hmirror(s, CONFIG["hmirror"]); //0 - 1
    s->set_colorbar(s, CONFIG["colorbar"]); //0 - 1
    s->set_special_effect(s, CONFIG["special_effect"]); // 0 - 6
    s->set_quality(s, CONFIG["quality"]); // 0 - 63
    s->set_brightness(s, CONFIG["brightness"]); // -2 - 2
    s->set_contrast(s, CONFIG["contrast"]); // -2 - 2
    s->set_saturation(s, CONFIG["saturation"]); // -2 - 2
    s->set_sharpness(s, CONFIG["sharpness"]); // -2 - 2
    s->set_denoise(s, CONFIG["denoise"]); // 0 - 1
    s->set_awb_gain(s, CONFIG["awb_gain"]); // 0 - 1
    s->set_wb_mode(s, CONFIG["wb_mode"]); // 0 - 4
  }
  if (topic == topic_BUFFERSIZE) {
    uint16_t newbuffer = messageTemp.toInt();

    if (client.setBufferSize(newbuffer)) {
      Serial.print("New buffer size set to ");
      Serial.println(client.getBufferSize());
    }
    else
    {
      Serial.print("failed setting new buffer size to ");
      Serial.println(newbuffer);
    }
  }

  if (topic == MQTT_TOPIC_LED_CMD) {
    if (messageTemp == "on") {
      Serial.println("LED on!");
      digitalWrite(LED, LOW);
    }
    else if (messageTemp == "off") {
      Serial.println("LED off!");
      digitalWrite(LED, HIGH);
    }
    else
    {
      Serial.print("do not know what to do with >");
      Serial.print(messageTemp);
      Serial.println("<");
    }
  }

  if (topic == MQTT_TOPIC_FLASH_CMD) {
    if (messageTemp == "on") {
      Serial.println("Flashlight on!");
      digitalWrite(FLASHLIGHT, HIGH);
    }
    else if (messageTemp == "off") {
      Serial.println("Flashlight off!");
      digitalWrite(FLASHLIGHT, LOW);
    }
    else
    {
      Serial.print("do not know what to do with >");
      Serial.print(messageTemp);
      Serial.println("<");
    }
  }
}

void camera_init() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = FRAMESIZE_VGA; // QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
  config.jpeg_quality = 10;
  config.fb_count     = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void take_picture() {
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  if (MQTT_MAX_PACKET_SIZE == 128) {
    //SLOW MODE (increase MQTT_MAX_PACKET_SIZE)
    //client.publish_P(topic_UP, fb->buf, fb->len, false);
    Serial.println("publish_P");
    client.publish(topic_UP, fb->buf, fb->len, false);
  }
  else {
    //FAST MODE (increase MQTT_MAX_PACKET_SIZE)
    Serial.println("publish");
    client.publish(topic_UP, fb->buf, fb->len, false);
  }
  client.publish(topic_STATUS, "CLIC");
  Serial.println("CLIC");

  esp_camera_fb_return(fb);
}
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    for (int i = 0; i <= 5; i++) {
      digitalWrite(LED, LOW);
      delay(50);
      digitalWrite(LED, HIGH);
      delay(50);
    }
    Serial.println("");
    Serial.println("WiFi connected");
    digitalWrite(LED, LOW);
    delay(1500);
    digitalWrite(LED, HIGH);
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(WIFI_HOSTNAME, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
      client.subscribe(topic_PHOTO);
      client.subscribe(topic_CONFIG);
      client.subscribe(topic_BUFFERSIZE);
      client.subscribe(MQTT_TOPIC_LED_CMD);
      client.subscribe(MQTT_TOPIC_FLASH_CMD);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      for (int i = 0; i <= 10; i++) {
        digitalWrite(LED, HIGH);
        delay(150);
        digitalWrite(LED, LOW);
        delay(50);
      }
    }
  }
}
void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(FLASHLIGHT, OUTPUT);

  camera_init();
  setup_wifi();
  
  //MQTT setup
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  //one flash to indicate readiness
  digitalWrite(FLASHLIGHT, HIGH);
  delay(50);
  digitalWrite(FLASHLIGHT, LOW);

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
