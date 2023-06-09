// Slightly adapted by Mark B. Jones - Scuba Hacker - for underwater testing of ESPNow
// Date: 20th May 2023
// https://github.com/scuba-hacker
// Original Authorship notice and instructions below.
// Thank you Arvind!
//
// Thank you also to Hague Nusseck @ electricidea for the M5 Stick C Text Buffer Scroller
// https://github.com/electricidea/M5StickC-TB_Display

/**
   ESPNOW - Basic communication - Slave
   Date: 26th September 2017
   Author: Arvind Ravulavaru <https://github.com/arvindr21>
   Purpose: ESPNow Communication between a Master ESP32 and multiple ESP32 Slaves
   Description: This sketch consists of the code for the Slave module.
   Resources: (A bit outdated)
   a. https://espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf
   b. http://www.esploradores.com/practica-6-conexion-esp-now/

   << This Device Slave >>

   Flow: Master
   Step 1 : ESPNow Init on Master and set it in STA mode
   Step 2 : Start scanning for Slave ESP32 (we have added a prefix of `slave` to the SSID of slave for an easy setup)
   Step 3 : Once found, add Slave as peer
   Step 4 : Register for send callback
   Step 5 : Start Transmitting data from Master to Slave(s)

   Flow: Slave
   Step 1 : ESPNow Init on Slave
   Step 2 : Update the SSID of Slave with a prefix of `slave`
   Step 3 : Set Slave in AP mode
   Step 4 : Register for receive callback and wait for data
   Step 5 : Once data arrives, print it in the serial monitor

   Note: Master and Slave have been defined to easily understand the setup.
         Based on the ESPNOW API, there is no concept of Master and Slave.
         Any devices can act as master or salve.
*/

#include <M5StickCPlus.h>

#include "tb_display.h"

#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  String Prefix = "Slave:";
  String Mac = WiFi.macAddress();
  String SSID = Prefix + Mac;
  String Password = "123456789";
  bool result = WiFi.softAP(SSID.c_str(), Password.c_str(), CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

// screen Rotation values:
// 1 = Button right
// 2 = Button above
// 3 = Button left
// 4 = Button below

uint8_t screen_orientation = 3;

uint8_t chosenTextSize = 2;

uint16_t packetsSent = 0;
uint16_t packetsReceived = 0;

void setup() {
  M5.begin();

  M5.Axp.ScreenBreath(14);             // max brightness

  M5.Lcd.setTextSize(chosenTextSize);

  // init the text buffer display and print welcome text on the display
  Serial.printf("textsize=%i",M5.Lcd.textsize);
  tb_display_init(screen_orientation,M5.Lcd.textsize);

  tb_display_print_String("Mercator Origins - Slave ESPNow Testbed\n");
  delay(1000);
  tb_display_print_String("Wait 15 seconds before ESPNow startup.");
  delay(15000);
  tb_display_print_String("ESPNow starting up...");

  Serial.begin(115200);
  Serial.println("ESPNow/Basic/Slave Example");
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
}

char buffer[100];

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  Serial.println("");

//  tb_display_print_String("Last Packet Recv from: "); 
//  tb_display_print_String(macStr);
  tb_display_print_String("Last Packet Recv Data: "); 

  sprintf(buffer, "'%c' ",*data);
  tb_display_print_String(buffer);
  tb_display_print_String("\n");
  
}

void loop() {
  // Chill
}
