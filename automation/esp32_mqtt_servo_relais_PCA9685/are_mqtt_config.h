#define MQTT_SERVER "broker.hivemq.com";
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_USER NULL //--> Create this in credentials.h
#define MQTT_PASSWORD NULL //--> Create this in credentials.h

#define MQTT_SUBSCRIBE_TOPIC "red5alex/valve01/command/#"  // make sure to choose the correct filter

#define MQTT_LASTWILL_TOPIC "red5alex/valve01/lastwill"
#define MQTT_LASTWILL_MESSAGE "connection lost"
#define MQTT_LASTWILL_QOS 0
#define MQTT_LASTWILL_RETAIN false
