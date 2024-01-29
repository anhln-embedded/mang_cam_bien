#include "all_header.h"

#define DHT_PIN 32
#define SENSOR_LIGHT_PIN 35
#define SOIL_PIN 34
#define DHTTYPE DHT21
#define LIGHT_PIN 25
#define PUMP_PIN 33

DHT dht(DHT_PIN, DHTTYPE);
DynamicJsonDocument data(1024);
char Buffer[1000];
float dht_temp = 0, dht_hum = 0, last_dht_temp = 0, last_dht_hum = 0;
int ss_light = 0, soil = 0, last_ss_light = 0, last_soil = 0,count_dht = 0, count_light = 0, count_soil = 0, Length_Buffer;
uint8_t light_state = 1, pump_state = 1;

void sensor_setup()
{
    dht.begin();
}

void sensor_dht_read()
{
    count_dht++;
    if(count_dht == 1000)
    {
        dht_hum = dht.readHumidity();
        dht_temp = dht.readTemperature();
        count_dht = 0;
    }
}

void sensor_light_read()
{
    float tmp;
    count_light++;
    // ss_light += analogRead(SENSOR_LIGHT_PIN);
    if(count_light == 1000)
    {
        tmp = 100 - analogRead(SENSOR_LIGHT_PIN) * 100 / 4095;
        ss_light = (int) tmp;
        count_light = 0;
    }
}

void sensor_soil_read()
{
    float tmp;
    count_soil++;
    // soil += analogRead(SOIL_PIN);
    if(count_soil == 1000)
    {
        tmp = 100 - analogRead(SOIL_PIN) * 100 / 4095;
        soil = (int) tmp;
        count_soil = 0;
        // soil = 0;
    }
    if(soil > 10)
    {
        soil *= 2;
    }
}


uint8_t filter_data()
{
    uint32_t flag = 0;
    int tmp;
    if(last_dht_hum - dht_hum >= 5 || last_dht_hum - dht_hum <= -5) //do am kk cu lech 5% voi do am kk moi update data
    {
        last_dht_hum = dht_hum;
        flag = 1;
    }
    if(last_dht_temp - dht_temp >= 1 || last_dht_temp - dht_temp <= -1)//nhiet do cu lech 1 do voi nhiet moi update data
    {
        last_dht_temp = dht_temp;
        flag = 1;
    }
    tmp = last_ss_light - ss_light;
    if(tmp >= 5 || tmp <= -5)// cuong do as cu lech 5% voi cuong do as moi update data
    {
        last_ss_light = ss_light;
        flag = 1;
    }
    tmp = last_soil - soil;
    if(tmp >= 5 || tmp <= -5)//do am dat cu lech 5% voi do am dat moi update data
    {
        last_soil = soil;
        flag = 1;
    }
    return flag;
}

void device_setup()
{
    pinMode(LIGHT_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(LIGHT_PIN, light_state);
    digitalWrite(PUMP_PIN, pump_state);
}
void device_light_control()
{
   if(ss_light < 30 && light_state == 1)
   {
       light_state = !light_state;
       digitalWrite(LIGHT_PIN, light_state);
   }
   else if(ss_light >= 30 && light_state == 0)
   {
        light_state = !light_state;
        digitalWrite(LIGHT_PIN, light_state);
	}
}
void device_pump_control()
{
   if(soil < 55 && pump_state == 1)
   {
        pump_state = !pump_state;
        digitalWrite(PUMP_PIN, pump_state);
   }
   else if(soil > 70 && pump_state == 0)
   {
        pump_state = !pump_state;
        digitalWrite(PUMP_PIN, pump_state);
   }
}

String Data_to_Json() {
    String ESP32_Data = "{\"temp\":\"" + String(last_dht_temp) + "\"," +
                      "\"humi\":\"" + String(last_dht_hum) + "\"," +
                      "\"soil\":\"" + String(last_soil) + "\"," +
                      "\"ss_light\":\"" + String(last_ss_light) + "\"," +
                      "\"light\":\"" + String(!light_state) + "\"," +
                      "\"pump\":\"" + String(!pump_state) + "\"}";
//   Length_Buffer = ESP32_Data.length()+1;
//   ESP32_Data.toCharArray(Buffer, Length_Buffer);
// //   Serial.println(Buffer);
  return ESP32_Data;
}
