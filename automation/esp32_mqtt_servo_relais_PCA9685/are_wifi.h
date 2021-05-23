#include <WiFi.h>

void connect_to_wifi(const char* ssid, const char* password, const char* hostname ) {
  Serial.print("Connecting to WiFi...");
  WiFi.disconnect(true);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.begin(ssid, password);
  WiFi.setHostname(hostname);
  while (WiFi.status() != WL_CONNECTED) { // Flash LED while connecting to WiFi
    for (int i = 0; i <= 5; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(50);
      digitalWrite(LED_BUILTIN, LOW);
      delay(50);
    }
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
}
