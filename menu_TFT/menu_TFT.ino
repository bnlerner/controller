#define cs   10
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset
#define selectButton A3
//#define 

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <Wire.h> // for reading off the barometric pressure
#include <avr/pgmspace.h> // for progmem to write strings to flash memory instead of sram
#include <MenuBackend.h>


Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

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

char* opti_lawn = "Welcome to Opti-lawn!";
// menu names
PROGMEM prog_char TopMenu[] = "Main Menu";
PROGMEM prog_char BottomMenu[] = "Settings";
// table to refer to strings
PROGMEM const char *string_table[] = 
{
  TopMenu,
  BottomMenu };

char buffer[50]; // size of largest string

//this controls the menu backend and the event generation
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent);
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
void menuUseEvent(MenuUseEvent used)
{
	Serial.print("Menu use ");
	Serial.println(used.item.getName());
	if (used.item == setDelay) //comparison agains a known item
	{
		Serial.println("menuUseEvent found Dealy (D)");
	}
}
void menuChangeEvent(MenuChangeEvent changed)
{
	Serial.print("Menu change ");
	Serial.print(changed.from.getName());
	Serial.print(" ");
	Serial.println(changed.to.getName());
}

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

char HIH4030_Pin = A2;

void setup(void) {
  Serial.begin(9600);
  menuSetup();
  Wire.begin();
  bmp085Calibration();
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(45);
  tft.fillScreen(ST7735_BLACK);
  testdrawtext(opti_lawn, ST7735_WHITE);
  delay(2000);
  
}

void loop() {
  //tft.fillScreen(ST7735_BLACK);
  //tft.drawRect(2,2,tft.width()-2,tft.height()-2,ST7735_GREEN);
  //showSensorReadings();
  if (Serial.available()) {
		byte read = Serial.read();
		switch (read) {
			case 'w': menu.moveUp(); break;
			case 's': menu.moveDown(); break;
			case 'd': menu.moveRight(); break;
			case 'a': menu.moveLeft(); break;
			case 'e': menu.use(); break;
		}
	}




}


void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(2, 4);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}


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
  int lineHeight = 4;
  tft.setCursor(4, lineHeight);
  lineHeight += 8;
  tft.fillRoundRect(4, 4, 80, 10, 5, ST7735_RED);
  tft.print("Temp: ");
  tft.print(Fahren,2);
  tft.println(" F");
  tft.setCursor(4, lineHeight);
  lineHeight += 8;
  tft.print("Press: ");
  tft.print(pressure, DEC);
  tft.println(" Pa");
  tft.setCursor(4, lineHeight);
  lineHeight += 8;
  
  altitude = (float)44330 * (1 - pow(((float) pressure/p0), 0.190295));
  float relativeHumidity  = getHumidity((float)temperature/10);   // humidity
  //Serial.print("Altitude: ");
  //Serial.print(altitude, 2);
  //Serial.println(" m");
  

  tft.print("Humidity:  ");
  tft.println(relativeHumidity,1);
  tft.setCursor(4, lineHeight);
  lineHeight += 8;
  tft.print("Pres Diff: ");
  tft.println(weatherDiff,0);
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
