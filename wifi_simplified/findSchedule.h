WiFiClient client;
char server[] = "www.opti-lawn.com";
char location[] = "http://opti-lawn.com";

void CloseAll() {
    digitalWrite(ZONE1,HIGH);
    digitalWrite(ZONE2,HIGH);
    digitalWrite(ZONE3,HIGH);
    digitalWrite(ZONE4,HIGH);
    digitalWrite(ZONE5,HIGH);
    digitalWrite(ZONE6,HIGH);
    digitalWrite(PUMP,HIGH);
    Serial.println("Zones Off");
  }


// function will turn on and off the sprinkler system
void ReadCommand(char* fromServer) {

  if(fromServer[1] == '1' && fromServer[3] == '1') {          // Zone 1
    digitalWrite(ZONE1,LOW);
    digitalWrite(ZONE2,HIGH);
    digitalWrite(ZONE3,HIGH);
    digitalWrite(ZONE4,HIGH);
    digitalWrite(ZONE5,HIGH);
    digitalWrite(ZONE6,HIGH);
    digitalWrite(PUMP,LOW);
    Serial.println("Zone 1 On");
  } else if(fromServer[1] == '2' && fromServer[3] == '1') {     // Zone 2
    digitalWrite(ZONE2,LOW);
    digitalWrite(ZONE1,HIGH);
    digitalWrite(ZONE3,HIGH);
    digitalWrite(ZONE4,HIGH);
    digitalWrite(ZONE5,HIGH);
    digitalWrite(ZONE6,HIGH);
    digitalWrite(PUMP,LOW);
    Serial.println("Zone 2 On");
  } else if(fromServer[1] == '3' && fromServer[3] == '1') {        // Zone 3
    digitalWrite(ZONE3,LOW);
    digitalWrite(ZONE2,HIGH);
    digitalWrite(ZONE1,HIGH);
    digitalWrite(ZONE4,HIGH);
    digitalWrite(ZONE5,HIGH);
    digitalWrite(ZONE6,HIGH);
    digitalWrite(PUMP,LOW);
    Serial.println("Zone 3 On");
  } else if(fromServer[1] == '4' && fromServer[3] == '1') {        // Zone 4
    digitalWrite(ZONE4,LOW);
    digitalWrite(ZONE2,HIGH);
    digitalWrite(ZONE3,HIGH);
    digitalWrite(ZONE1,HIGH);
    digitalWrite(ZONE5,HIGH);
    digitalWrite(ZONE6,HIGH);
    digitalWrite(PUMP,LOW);
    Serial.println("Zone 4 On");
  } else if(fromServer[1] == '5' && fromServer[3] == '1') {        // Zone 5
    digitalWrite(ZONE5,LOW);
    digitalWrite(ZONE2,HIGH);
    digitalWrite(ZONE3,HIGH);
    digitalWrite(ZONE1,HIGH);
    digitalWrite(ZONE4,HIGH);
    digitalWrite(ZONE6,HIGH);
    digitalWrite(PUMP,LOW);
    Serial.println("Zone 5 On");
  } else if(fromServer[1] == '6' && fromServer[3] == '1') {        // Zone 6
    digitalWrite(ZONE6,LOW);
    digitalWrite(ZONE2,HIGH);
    digitalWrite(ZONE3,HIGH);
    digitalWrite(ZONE1,HIGH);
    digitalWrite(ZONE5,HIGH);
    digitalWrite(ZONE4,HIGH);
    digitalWrite(PUMP,LOW);
    Serial.println("Zone 6 On");
  }
  else {
    CloseAll(); 
  }
  
  if(fromServer[3] == '9') {
    resetFunc();
  }
}

//function will read schedule from server
void determineSchedule() {
  
   Serial.println("connecting...");

  //port 80 is typical of a www page
  if (client.connect(server, 80)) {
    Serial.println("connected");
    client.print("GET ");
    client.print(location);
    client.print("/WateringSchedule.php?");
    client.print("&deviceCode=");
    client.print(serialNumber);
    client.println(" HTTP/1.1");
    client.println("Host: www.opti-lawn.com");
    client.println("Connection: close");
    client.println();
    
    int start = 0;
    int i = 0;
    char command[] = "";
    while (client.connected() || client.available()) { //connected or data available
      char c = client.read();
      
      if (c == '{' || start == 1) {
        Serial.print(c);
        command[i] = c;
        start = 1;
        i++;
        
        if(c == '}') { break; }
      }
  }
    
    //delay(500);
    Serial.println(command);
    ReadCommand(command);

    client.flush();
    client.stop();
    Serial.println("End of Find Schedule");
  }else{
    Serial.println("connection failed");
    Serial.println("Reseting board");
    resetFunc(); 
  }
}




