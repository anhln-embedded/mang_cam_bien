#include "all_header.h"

uint32_t count;
void setup()
{
  Serial.begin(115200);
  mesh_setup();
  sensor_setup();
  device_setup();
  count = millis();
}

void loop()
{
  mesh_loop();

  // if(millis() > count + 5000)
  // {
  //   count = millis();
  //   Serial.println(Data_to_Json());
  // } 
}

