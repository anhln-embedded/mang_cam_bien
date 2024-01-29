#ifndef _ALL_HEADER_
#define _ALL_HEADER_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "DHT.h"

void mesh_setup();
void mesh_loop();
void sensor_setup();
void sensor_dht_read();
void sensor_light_read();
void sensor_soil_read();
void device_setup();
void device_light_control();
void device_pump_control();
uint8_t filter_data();
String Data_to_Json();
#endif