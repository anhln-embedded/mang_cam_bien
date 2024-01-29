
#include "all_header.h"
#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>
// In this small artifical network of 4 nodes,
#define BRIDGE_ADDRESS 1  // address of the bridge ( we send our data to, hopefully the bridge knows what to do with our data )
#define NODE_ADDRESS   3    // address of this node
#define RH_MESH_MAX_MESSAGE_LEN 100
// lilygo T3 v2.1.6
// lora SX1276/8
#define LLG_SCK 18
#define LLG_MISO 19
#define LLG_MOSI 23
#define LLG_CS  5
#define LLG_RST 14
#define LLG_DI0 2
// Dont put this on the stack:
uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];
uint8_t res;

#define LLG_LED_GRN 25
#define TXINTERVAL 5000  // delay between successive transmissions
unsigned long nextTxTime;
// Singleton instance of the radio driver
RH_RF95 rf95(LLG_CS, LLG_DI0);
RHMesh manager(rf95, NODE_ADDRESS);
void mesh_setup() 
{
    Serial.begin(115200);
    Serial.print(F("initializing node "));
    Serial.print(NODE_ADDRESS);
    SPI.begin(LLG_SCK,LLG_MISO,LLG_MOSI,LLG_CS);
    if (!manager.init())
        {Serial.println(" init failed");} 
    else
        {Serial.println(" done");}  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36 
    rf95.setTxPower(10, false); // with false output is on PA_BOOST, power from 2 to 20 dBm, use this setting for high power demos/real usage
    //rf95.setTxPower(1, true); // true output is on RFO, power from 0 to 15 dBm, use this setting for low power demos ( does not work on lilygo lora32 )
    rf95.setFrequency(433.0);
    rf95.setCADTimeout(500);
    // long range configuration requires for on-air time
    boolean longRange = false;
    if (longRange) 
    {
        // custom configuration
        RH_RF95::ModemConfig modem_config = {
            0x78, // Reg 0x1D: BW=125kHz, Coding=4/8, Header=explicit
            0xC4, // Reg 0x1E: Spread=4096chips/symbol, CRC=enable
            0x08  // Reg 0x26: LowDataRate=On, Agc=Off.  0x0C is LowDataRate=ON, ACG=ON
        };
        rf95.setModemRegisters(&modem_config);
    }
    else
    {
        // Predefined configurations( bandwidth, coding rate, spread factor ):
        // Bw125Cr45Sf128     Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range
        // Bw500Cr45Sf128     Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range
        // Bw31_25Cr48Sf512   Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range
        // Bw125Cr48Sf4096    Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, low data rate, CRC on. Slow+long range
        // Bw125Cr45Sf2048    Bw = 125 kHz, Cr = 4/5, Sf = 2048chips/symbol, CRC on. Slow+long range
        if (!rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128))
        {
            Serial.println(F("set config failed"));
        }
    }
    Serial.println("RF95 ready");
    nextTxTime = millis();
}

void mesh_loop()
{
    // send message every TXINTERVAL millisecs
    if (millis() - nextTxTime > TXINTERVAL  && filter_data() == 1)
    // if (millis() > nextTxTime)    
    {
        uint8_t data_json[100];
        String data_json_char = Data_to_Json();
        memcpy(data_json, data_json_char.c_str(), data_json_char.length());
        nextTxTime = millis();
        Serial.print("Sending to bridge ");
        Serial.print(BRIDGE_ADDRESS);
        Serial.print(" res=");
        // Send a message to a rf95_mesh_server
        // A route to the destination will be automatically discovered.
        data_json[data_json_char.length()] = NULL;
        res = manager.sendtoWait(data_json, sizeof(data_json), BRIDGE_ADDRESS);
        Serial.println(res);
        if (res == RH_ROUTER_ERROR_NONE)
        {
        // Data has been reliably delivered to the next node.
        // now we do... 
        }
        else
        {
        // Data not delivered to the next node.
        Serial.println("sendtoWait failed. Are the bridge/intermediate mesh nodes running?");
        }
        }
        // radio needs to stay always in receive mode ( to process/forward messages )
        uint8_t len = sizeof(buf);
        uint8_t from;
        if (manager.recvfromAck(buf, &len, &from))
        {
        Serial.print("message from node ");
        Serial.print(from);
        Serial.print(": ");
        Serial.print((char*)buf);
        Serial.print(" rssi: ");
        Serial.println(rf95.lastRssi()); 
    }
        sensor_dht_read();
        sensor_light_read();
        sensor_soil_read();
        device_light_control();
        device_pump_control();  
}
