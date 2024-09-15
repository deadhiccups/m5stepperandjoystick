/**
   Receiver Module that process ESP-NOW command to 
   turn stepper motor clockwise or counterclockwise
   M5 Core Module with GRBL module
*/

#include <esp_now.h>
#include <WiFi.h>
#include <M5Unified.h>
#include "Module_GRBL_13.2.h"

#define CHANNEL 1
#define STEPMOTOR_I2C_ADDR 0x70
// #define STEPMOTOR_I2C_ADDR 0x71

Module_GRBL _GRBL = Module_GRBL(STEPMOTOR_I2C_ADDR);


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
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
    Serial.print("AP CHANNEL "); Serial.println(WiFi.channel());
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("ESPNow/Basic/Slave Example");
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  M5.begin();
  M5.Power.begin();
  Wire.begin(21, 22);
  _GRBL.Init(&Wire);
  //_GRBL.setMode("absolute");
  _GRBL.setMode("distance");


  M5.Lcd.setCursor(0, 25);
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setBrightness(200);
  M5.Lcd.setTextSize(2);
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);

  M5.Lcd.print("espnow slave: ");
  M5.Lcd.print(WiFi.softAPmacAddress());

  M5.Lcd.setCursor(27, 50, 2);
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  M5.update();
  char macStr[18];
  M5.Lcd.setTextSize(2);
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  M5.Lcd.printf("Data::%c ", *data);
  if (*data == 'L') {
    _GRBL.sendGcode("G1 X0Y0Z.05 F400");
  }
  if (*data == 'R') {
    _GRBL.sendGcode("G1 X0Y0Z-.05 F400");
  }
  M5.update();
  delay(50);

}

void loop() {
  M5.update();
  
  delay(50);

 }
