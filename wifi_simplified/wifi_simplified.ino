/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
*/
// set up wifi from Brians apartment
#include <Time.h>
#include <WiFi.h>
#include <SPI.h>
//#include <SD.h>
//#include <MemoryFree.h>
//#include <SubMenuItem.h>
//#include <SubMenu.h>
//#include <MenuItem.h>
//#include <Menu.h>

const int ZONE1 = 2;
const int ZONE2 = 3;
const int ZONE3 = 4;
const int ZONE4 = 5;
const int ZONE5 = 6;
const int ZONE6 = 8;
const int PUMP = 9;

//char serialNumber[] = "234234"; // need hardware serial number
char serialNumber[] = "111111"; // need hardware serial number from me
boolean wifiOn = false;
unsigned long startTime = millis();// time since started in milliseconds

// initialize ip
IPAddress ip = WiFi.localIP();

// command
char command[] = "";

void(* resetFunc) (void) = 0; //declare reset function @ address 0

int freeRam () { // show free ram
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

//include headers
//#include "writeWifiData.h";
#include "connectToWifi.h";
//#include "connectToServer.h";
#include "findSchedule.h";
//#include "SDcard.h";
//char fromServer[] = {1,1}; 


void setup() {
  wifiOn = false;
  pinMode(ZONE1, OUTPUT);
  pinMode(ZONE2 ,OUTPUT);
  pinMode(ZONE3 ,OUTPUT);
  pinMode(ZONE4, OUTPUT);
  pinMode(ZONE5, OUTPUT);
  pinMode(ZONE6, OUTPUT);
  pinMode(PUMP, OUTPUT);
  digitalWrite(ZONE1,HIGH);
  digitalWrite(ZONE2,HIGH);
  digitalWrite(ZONE3,HIGH);
  digitalWrite(ZONE4,HIGH);
  digitalWrite(ZONE5,HIGH);
  digitalWrite(ZONE6,HIGH);
  digitalWrite(PUMP,HIGH);
  Serial.begin(9600);
  
  
  connectToWifi();
  //printWifiData();
}

void loop() {
  //connectAndRead(moistureValue, pHValue, serialNumber); //connect to the server and read the output
  if(status = WL_CONNECTED) {
    //Serial.println("connected");
    determineSchedule(); // pulls schedule from server
    //Serial.println("exit determine Schedule");
  } else {
    Serial.println("WiFi not connected. attempting to reconnect");
    delay(500);
    connectToWifi();
    wifiOn = false;    
  }
  delay(10000);
  // add functionality to save to SD card
  //void saveData(dataFile)
  startTime = millis();
  //Serial.print("Time since restart: ");
  //Serial.print(startTime/1000);
  //Serial.println(" Seconds");
  if((startTime > 21600000 || freeRam() < 50) && command[3] != '1') { // functionality to reset every 6 hours or if ram less than 50
    Serial.println("resetting because over 6 hours");
    delay(500);
    resetFunc();
  }
}
/// menu options

