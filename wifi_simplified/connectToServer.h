// to connect to server

//MAC Address MAC address: 7A:C4:0E:1C:A9:BB
WiFiClient client;
char server[] = "www.opti-lawn.com";
char location[] = "http://opti-lawn.com";

void readPage(){
    while(client.connected() && !client.available()) {
      delay(1); //waits for data
    }
    while (client.connected() || client.available()) { //connected or data available
      char c = client.read();
      Serial.print(c);
  }

  Serial.println();
  Serial.println("disconnecting.");
  Serial.println("==================");
  Serial.println();
  client.flush();
  client.stop();
}

void connectAndRead(float soilMoisture, float pH, char* serial){
  //connect to the server

  Serial.println("connecting...");

  //port 80 is typical of a www page
  if (client.connect(server, 80)) {
    Serial.println("connected");
    client.print("GET ");
    client.print(location);
    client.print("/insertsensordata.php?");
    client.print("&soilMoisture=");
    client.print(soilMoisture);
    client.print("&pH=");
    client.print(pH);
    client.print("&deviceCode=");
    client.print(serial);
    client.print("&deviceIP=");
    client.print(ip);
    client.println(" HTTP/1.1");
    client.println("Host: www.opti-lawn.com");
    client.println("Connection: close");
    client.println();
    
    readPage(); //go and read the output

  }else{
    Serial.println("connection failed");
    client.flush();
  }
}


