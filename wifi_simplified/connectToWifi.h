//char ssid[] = "ATT008";     //  your network SSID (name) 
//char pass[] = "9600732023";  // your network password
int keyIndex = 0;                 // your network key Index n

char ssid[] = "2WIRE686";
char pass[] = "9305591084";

//char ssid[] = "GTother";  
//char pass[] = "GeorgeP@1927";

//umber (needed only for WEP)
int status = WL_IDLE_STATUS;     // the Wifi radio's status

void connectToWifi() {
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  }
  
 // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:    
    status = WiFi.begin(ssid, pass);

    // wait 5 seconds for connection:
    delay(5000);
  }
   
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  //printCurrentNet();
  //printWifiData();
}
