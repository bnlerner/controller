/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 
 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */
 
 // include the library code:
#include <LiquidCrystal.h>
//#include <MenuBackend.h>
//#include <Time.h>
#include <avr/pgmspace.h> // for progmem to write strings to flash memory instead of sram
//#include <EEPROMex.h> // storing values in the "hard drive"
#include <Wire.h> // for reading off the barometric pressure
//#include <SPI.h>
//#include "UdpByteWise.h"  // UDP library from: bjoern@cs.stanford.edu 12/30/2008 

//////////////////////////////////////////////////////////////////////////////////////////////////
//SENSORS/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

#define BMP085_ADDRESS 0x77  // I2C address of BMP085

const unsigned char OSS = 0;  // Oversampling Setting

// Calibration values
int ac1;
int ac2; 
int ac3; 
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1; 
int b2;
int mb;
int mc;
int md;

// b5 is calculated in bmp085GetTemperature(...), this variable is also used in bmp085GetPressure(...)
// so ...Temperature(...) must be called before ...Pressure(...).
long b5; 

float altitude;
const float p0 = 101325;     // Pressure at sea level (Pa)
const float currentAltitude = 273.00; // current altitude in METERS
const float ePressure = p0 * pow((1-currentAltitude/44330), 5.255);

float weatherDiff;

short temperature;
long pressure;

char HIH4030_Pin = A0;

//////////////////////////////////////////////////////////////////////////////////////////////////
/////MENU/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

/*
	This is the structure of the modelled menu
	
	Settings
		Pin
		Debug
	Options
		Delay (D)
			100 ms
			200 ms
			300 ms
			400 ms
*/

//this controls the menu backend and the event generation
//MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent);

// menu names
PROGMEM prog_char TopMenu[] = "First!";
PROGMEM prog_char BottomMenu[] = "Second!!";

// table to refer to strings
PROGMEM const char *string_table[] = 
{
  TopMenu,
  BottomMenu };

char buffer[50]; // size of largest string
/*
//beneath is list of menu items needed to build the menu
	MenuItem settings = MenuItem("Settings");
		MenuItem pin = MenuItem("Pin");
		MenuItem debug = MenuItem("Debug");
	MenuItem options = MenuItem("Options");
		MenuItem setDelay = MenuItem("Delay",'D');
			MenuItem d100 = MenuItem("100 ms");
			MenuItem d200 = MenuItem("200 ms");
			MenuItem d300 = MenuItem("300 ms");
			MenuItem d400 = MenuItem("400 ms");

//this function builds the menu and connects the correct items together
void menuSetup()
{
	Serial.println("Setting up menu...");
	//add the file menu to the menu root
	menu.getRoot().add(settings); 
		//setup the settings menu item
		settings.addRight(pin);
			//we want looping both up and down
			pin.addBefore(debug);
			pin.addAfter(debug);
			debug.addAfter(pin);
			//we want a left movement to pint to settings from anywhere
			debug.addLeft(settings);
			pin.addLeft(settings);
	settings.addBefore(options);
	settings.addAfter(options);
		options.addRight(setDelay);
			setDelay.addLeft(options);
			setDelay.addRight(d100);
				d100.addBefore(d100); //loop to d400 
				d100.addAfter(d200);
				d200.addAfter(d300);
				d300.addAfter(d400);
				d400.addAfter(d100); //loop back to d100
				//we want left to always be bak to delay
				d100.addLeft(setDelay);
				d200.addLeft(setDelay);
				d300.addLeft(setDelay);
				d400.addLeft(setDelay);
	options.addAfter(options);
}
*/
/*
	This is an important function
	Here all use events are handled
	
	This is where you define a behaviour for a menu item
*/
/*
void menuUseEvent(MenuUseEvent used)
{
	Serial.print("Menu use ");
	Serial.println(used.item.getName());
	if (used.item == setDelay) //comparison agains a known item
	{
		Serial.println("menuUseEvent found Dealy (D)");
	}
}
*/
/*
	This is an important function
	Here we get a notification whenever the user changes the menu
	That is, when the menu is navigated
*/
/*
void menuChangeEvent(MenuChangeEvent changed)
{
	Serial.print("Menu change ");
	Serial.print(changed.from.getName());
	Serial.print(" ");
	Serial.println(changed.to.getName());
}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////
/////LCD//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 5, 4, 3, 2);
/*
/// current time
byte SNTP_server_IP[]    = { 192, 43, 244, 18}; // time.nist.gov
time_t t = now();
/*-------- NTP code ----------*/
/*
unsigned long getNtpTime()
{
  sendNTPpacket(SNTP_server_IP);
  delay(1000);
  if ( UdpBytewise.available() ) {
    for(int i=0; i < 40; i++)
       UdpBytewise.read(); // ignore every field except the time
    const unsigned long seventy_years = 2208988800UL + timeZoneOffset;        
    return getUlong() -  seventy_years;      
  }
  return 0; // return 0 if unable to get the time
}

unsigned long sendNTPpacket(byte *address)
{
  UdpBytewise.begin(123);
  UdpBytewise.beginPacket(address, 123);
  UdpBytewise.write(B11100011);   // LI, Version, Mode
  UdpBytewise.write(0);    // Stratum
  UdpBytewise.write(6);  // Polling Interval
  UdpBytewise.write(0xEC); // Peer Clock Precision
  write_n(0, 8);    // Root Delay & Root Dispersion
  UdpBytewise.write(49); 
  UdpBytewise.write(0x4E);
  UdpBytewise.write(49);
  UdpBytewise.write(52);
  write_n(0, 32); //Reference and time stamps  
  UdpBytewise.endPacket();   
}

unsigned long getUlong()
{
    unsigned long ulong = (unsigned long)UdpBytewise.read() << 24;
    ulong |= (unsigned long)UdpBytewise.read() << 16;
    ulong |= (unsigned long)UdpBytewise.read() << 8;
    ulong |= (unsigned long)UdpBytewise.read();
    return ulong;
}

void write_n(int what, int how_many)
{
  for( int i = 0; i < how_many; i++ )
    UdpBytewise.write(what);
}
*/


void setup() {
  lcd.begin(16, 2);

  Serial.begin(9600);
 
  lcd.clear();
  //Serial.println("waiting for sync");
  //setSyncProvider(getNtpTime);
  //while(timeStatus()== timeNotSet)   
  //vff   ; // wait until the time is set by the sync provider
  //t = now();
  // set up the LCD's number of columns and rows: 
  
  //menuSetup();
  // Print a message to the LCD.
  //lcd.print("hello, world!");
  
  Wire.begin();
  bmp085Calibration();
  
 // setTime(t);
}

void loop() {
  strcpy_P(buffer, (char*)pgm_read_word(&(string_table[0]))); // Necessary casts and dereferencing, just copy. 
  Serial.println(buffer);
  strcpy_P(buffer, (char*)pgm_read_word(&(string_table[1])));
  Serial.println(buffer);
  //Serial.println(timeStatus());
  //Serial.println(year(t));
  //showSensorReadings();
  delay(1000);
}


/////////////////functions

void showSensorReadings() {
  temperature = bmp085GetTemperature(bmp085ReadUT());
  Serial.println(temperature);
  float Fahren = (float)temperature*9/50+32;
  Serial.println(Fahren);
  pressure = bmp085GetPressure(bmp085ReadUP());
  Serial.println(pressure);
  weatherDiff = pressure - ePressure;
  /*
  For example, pressure changes of more than about -250 Pa per hour (Pa/h), result in 
  unstable weather patterns like thunderstorms, while lower rates, between -50 and -250 Pa/h 
  produce stable, though still rainy weather. 
  
  Usually sunny if above 250 difference, cloudy for even and rain for below -250
  */

  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  
  lcd.clear();
  lcd.print("Temp: ");
  lcd.print(Fahren,2);
  lcd.println(" F");
  lcd.setCursor(0, 1);
  lcd.print("Press: ");
  lcd.print(pressure, DEC);
  lcd.println(" Pa");
  delay(5000);
  
  altitude = (float)44330 * (1 - pow(((float) pressure/p0), 0.190295));
  float relativeHumidity  = getHumidity((float)temperature/10);   // humidity
  //Serial.print("Altitude: ");
  //Serial.print(altitude, 2);
  //Serial.println(" m");
  
  lcd.clear();
  lcd.print("Humidity:  ");
  lcd.print(relativeHumidity,1);
  lcd.setCursor(0, 1);
  lcd.print("Pres Diff: ");
  lcd.print(weatherDiff,0);
  delay(5000);
}


// Stores all of the bmp085's calibration values into global variables
// Calibration values are required to calculate temp and pressure
// This function should be called at the beginning of the program
void bmp085Calibration()
{
  ac1 = bmp085ReadInt(0xAA);
  ac2 = bmp085ReadInt(0xAC);
  ac3 = bmp085ReadInt(0xAE);
  ac4 = bmp085ReadInt(0xB0);
  ac5 = bmp085ReadInt(0xB2);
  ac6 = bmp085ReadInt(0xB4);
  b1 = bmp085ReadInt(0xB6);
  b2 = bmp085ReadInt(0xB8);
  mb = bmp085ReadInt(0xBA);
  mc = bmp085ReadInt(0xBC);
  md = bmp085ReadInt(0xBE);
}

// Calculate temperature given ut.
// Value returned will be in units of 0.1 deg C
short bmp085GetTemperature(unsigned int ut)
{
  long x1, x2;
  
  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  return ((b5 + 8)>>4);  
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
long bmp085GetPressure(unsigned long up)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;
  
  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;
  
  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;
  
  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;
    
  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;
  
  return p;
}

// Read 1 byte from the BMP085 at 'address'
char bmp085Read(unsigned char address)
{
  unsigned char data;
  
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_ADDRESS, 1);
  while(!Wire.available())
    ;
    
  return Wire.read();
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int bmp085ReadInt(unsigned char address)
{
  unsigned char msb, lsb;
  
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  
  return (int) msb<<8 | lsb;
}

// Read the uncompensated temperature value
unsigned int bmp085ReadUT()
{
  unsigned int ut;
  
  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  
  // Wait at least 4.5ms
  delay(5);
  
  // Read two bytes from registers 0xF6 and 0xF7
  ut = bmp085ReadInt(0xF6);
  return ut;
}

// Read the uncompensated pressure value
unsigned long bmp085ReadUP()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;
  
  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();
  
  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<OSS));
  
  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);
  
  // Wait for data to become available
  while(Wire.available() < 3)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();
  
  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);
  
  return up;
}


float getHumidity(float degreesCelsius){
  //caculate relative humidity
  float supplyVolt = 5.0;

  // read the value from the sensor:
  int HIH4030_Value = analogRead(HIH4030_Pin);
  float voltage = HIH4030_Value/1023. * supplyVolt; // convert to voltage value

  // convert the voltage to a relative humidity
  // - the equation is derived from the HIH-4030/31 datasheet
  // - it is not calibrated to your individual sensor
  //  Table 2 of the sheet shows the may deviate from this line
  float sensorRH = 161.0 * voltage / supplyVolt - 25.8;
  float trueRH = sensorRH / (1.0546 - 0.0026 * degreesCelsius); //temperature adjustment 

  return trueRH;
}

void readInput() {
  
//  int 
//  switch (read) {
//			case 'w': menu.moveUp(); break;
//			case 's': menu.moveDown(); break;
//			case 'd': menu.moveRight(); break;
//			case 'a': menu.moveLeft(); break;
//			case 'e': menu.use(); break;
//		}
}

void saveSchedule() {
//  EEPROM.write(0,);  // address, value
  
}

void readSchedule() {
// int value = EEPROM.read(0); // address
}



