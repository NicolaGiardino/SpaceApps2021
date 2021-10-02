#include "Arduino.h"
#include "DHT.h"
#include "GraphicLCD.h"
#include "Switchable.h"
#include "LED.h"
#include "Fan.h"
#include "Wire.h"
#include "RTClib.h"
#include "SoilMoisture.h"
#include "Pump.h"


// Pin Definitions
#define DHT_PIN_DATA	2
#define GRAPHICLCD_PIN_TX	4
#define GRAPHICLCD_PIN_RX	3
#define LEDSTRIPS_PIN_SIG	9
#define O2_AOUT	A3
#define PCFAN_PIN_COIL1	5
#define SOILMOISTURE_5V_PIN_SIG	A1
#define VACCUMPUMP_PIN_COIL1	6



// Global variables and defines

// object initialization
DHT dht(DHT_PIN_DATA);
GraphicLCD graphicLCD(GRAPHICLCD_PIN_RX,GRAPHICLCD_PIN_TX);
LED ledStripS(LEDSTRIPS_PIN_SIG);
Fan PCFan(PCFAN_PIN_COIL1);
RTC_DS3231 rtcDS;
SoilMoisture soilMoisture_5v(SOILMOISTURE_5V_PIN_SIG);
Pump vaccumpump(VACCUMPUMP_PIN_COIL1);


// define vars for testing menu
const int timeout = 10000;       //define timeout of 10 sec
char menuOption = 0;
long time0;

const int nightHour = 20;
const float lowHumidity = 80;
const int lowMoisture = 100;
const float highTemp = 27; 

// Setup the essentials for your circuit to work. It runs first every time your circuit is powered with electricity.
void setup() 
{
    // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.begin(9600);
    while (!Serial) ; // wait for serial port to connect. Needed for native USB
    Serial.println("start");
    
    dht.begin();
    if (! rtcDS.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
    }
    if (rtcDS.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtcDS.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    
}

void loop() 
{
    
    
    float dhtHumidity = dht.readHumidity();
    // Read temperature in Celsius
    float dhtTempC = dht.readTempC();
      

    if(dhtTempC > highTemp) {
        PCFan.off();
    }


    int O2 = analogRead(O2_AOUT);

    int soilMoisture = soilMoisture_5v.read();


    if(soilMoisture < lowMoisture || dhtHumidity < lowHumidity) {
        vaccumpump.on();
    } else {
        vaccumpump.off();
    }

    graphicLCD.setX(1);
    graphicLCD.setY(1);

    DateTime now = rtcDS.now();
    graphicLCD.print(now.month());
    graphicLCD.print('/');
    graphicLCD.print(now.day());
    graphicLCD.print('/');
    graphicLCD.print(now.year());
    graphicLCD.print("  ");
    graphicLCD.print(now.hour());
    graphicLCD.print(':');
    graphicLCD.print(now.minute());
    graphicLCD.print(':');
    graphicLCD.print(now.second());

    
    if(now.hour() > nightHour) {
        ledStripS.off();
    } else {
        ledStripS.on();
    }
    
    // The LCD Screen will display the text of your choice at the location (30,50) on screen. Counting from the top left corner: 30 pixels to the right and 50 pixels down  
    graphicLCD.setX(2);                     // 1. sets left-right indent for text to print. Change the value in the brackets (1 - left, 164 - right) for a different indent.
    graphicLCD.setY(1);                     // 2. sets top-bottom height for text to print. Change the value in the brackets (1 - top, 128 - bottom) for a different height.
    graphicLCD.print("Humidity");
    graphicLCD.print(dhtHumidity);
    graphicLCD.print("%");
    graphicLCD.setX(3);
    graphicLCD.setY(1);
    graphicLCD.print("Temperature");
    graphicLCD.print(dhtTempC);
    graphicLCD.print("C");


    delay(1000);                               // 4. waits 1000 milliseconds (1 sec). Change the value in the brackets (1000) for a longer or shorter delay in milliseconds.
    graphicLCD.clear();                      // 5. wipes the screen
    delay(100);
  
    
}
