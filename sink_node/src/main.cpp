
#include <Arduino.h>
#include "bridge.h"
#include "mqtt.h"

void callBackOnMessage(uint8_t *_buf, uint8_t _length, uint8_t _from);

void setup() 
{
  Serial.begin(115200);
  mqtt_init();
  bridge_setup();
}

void loop()
{
  mqtt_loop();
  bridge_loop(callBackOnMessage);
}

void callBackOnMessage(uint8_t *_buf, uint8_t _length, uint8_t _from){
  String JSON = "";
  JSON += "{";
  JSON += "\"address\":" + String(_from) + ",";
  JSON += "\"data\":" + String((char*)_buf);
  JSON += "}";
  mqtt_pub(MQTT_TOPIC_PUB, JSON);
  Serial.println(JSON); 
}