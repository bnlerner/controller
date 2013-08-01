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
#include <MemoryFree.h>

const int ZONE1 = 2;
const int ZONE2 = 3;
const int ZONE3 = 4;
const int ZONE4 = 5;
const int ZONE5 = 6;
const int ZONE6 = 8;
const int PUMP = 9;

char serialNumber[] = "234234"; // need hardware serial number
boolean wifiOn = false;
unsigned long startTime = millis();// time since started in milliseconds

// initialize ip
IPAddress ip = WiFi.localIP();

// command
char command[] = "";


void(* resetFunc) (void) = 0; //declare reset function @ address 0

//include headers
//#include "writeWifiData.h";
#include "connectToWifi.h";
//#include "connectToServer.h";
#include "Memory.h";
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
  
   // make sure that the default chip select pin is set to
  // output, even if you don't use it:
 // pinMode(10, OUTPUT);
  // if (!SD.begin(chipSelect)) {
    //Serial.println("Card failed, or not present");
    // don't do anything more:
 //   return;
 // }
}


void loop() {
  //connectAndRead(moistureValue, pHValue, serialNumber); //connect to the server and read the output
  if(!client.connected()) {
    connectToWifi();
    wifiOn = false;
  }
  
  
  
  determineSchedule(); // pulls schedule from server
  //Serial.println("exit determine Schedule");
  delay(10000);
// add functionality to reset every 6 hours
// add functionality to save to SD card
  //void saveData(dataFile)
  startTime = millis();
  if(startTime > 21600000) {
    resetFunc();
  }
}



 


