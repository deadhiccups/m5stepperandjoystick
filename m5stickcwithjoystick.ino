/*
*
* Joystick Example Code For Min JoyC Hat For Use With M5 StickC Plus2.
* displays joystick status - waiting, up, down, left, right, button press on display
* Joystick direction base on x and y position,  could be different on yours
* 
* send data to motor using espnow
* change brodcast address to mac address of core module receiving command
* Dead Hiccups
*/

#include <M5StickCPlus2.h>
#include "Wire.h"
#include "esp_now.h"
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x34, 0x98, 0x7A, 0x6C, 0x8B, 0xD5};

// Structure example to send data
// Must match the receiver structure

char message[1];

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 


#define JOY_ADDR 0x54  // define Joystick I2C address
#define BUTTON_ADDR 0x30 // button status register
#define XVALUE_ADDR 0x20 // x value status register
#define YVALUE_ADDR 0x21 // y value status register

void setup() {
    M5.begin();
    M5.Power.begin();
    M5.Lcd.setTextSize(2);
    M5.Lcd.setRotation(1);
    M5.Lcd.setCursor(7, 20, 2);
    dacWrite(25, 0);  // disable speaker noise
    // sda 0 scl 26
    Wire.begin(0, 26, 100000UL);
    WiFi.mode(WIFI_STA);
  // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
    return;

}
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
    uint8_t  button_data;
    uint8_t x_data;
    uint8_t  y_data;
    

    Wire.beginTransmission(JOY_ADDR);
    Wire.write(BUTTON_ADDR);
    Wire.endTransmission();
    
    Wire.requestFrom(
        JOY_ADDR,
        1);  // Request 1 bytes from the slave device. 
    while (Wire.available()){
        button_data = Wire.read();

    }
    Wire.beginTransmission(JOY_ADDR);
    Wire.write(XVALUE_ADDR);
    Wire.endTransmission();
    Wire.requestFrom(
        JOY_ADDR,
        1);  // Request 1 bytes from the slave device. 
    while (Wire.available()){
      x_data = Wire.read();

    }
    Wire.beginTransmission(JOY_ADDR);
    Wire.write(YVALUE_ADDR);
    Wire.endTransmission();
    Wire.requestFrom(
        JOY_ADDR,
        1);  // Request 1 bytes from the slave device. 
    while (Wire.available()){
      y_data = Wire.read();

    }
 
    M5.Lcd.setCursor(7, 20, 2);
//    uncomment this if you want you x and y value of your joystick    
//    M5.Lcd.setCursor(7, 55, 2);
//    M5.Lcd.printf("xvalue:  %d ",x_data);
//    M5.Lcd.setCursor(7, 85, 2);
//    M5.Lcd.printf("yvalue:  %d ",y_data);
    M5.Lcd.printf("WAITING         ");
    M5.Lcd.setCursor(7, 20, 2);
    if (x_data > 49 & x_data <110){
      M5.Lcd.printf("right       ");
      strcpy(message, "R");      
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &message, sizeof(message));

    }
    if (x_data > 125 & x_data <190){
      M5.Lcd.printf("left        ");
      strcpy(message, "L");      
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &message, sizeof(message));


    }
    if (y_data > 45 & y_data <115){
      M5.Lcd.printf("down        ");
      strcpy(message, "D");      
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &message, sizeof(message));

    }
    if (y_data > 140 & y_data <200){
      M5.Lcd.printf("up          ");
      strcpy(message, "U");      
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &message, sizeof(message));

    }
    if (button_data == 0){
      M5.Lcd.printf("button press");
      strcpy(message, "B");      
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &message, sizeof(message));

    }
    delay(200);
}
