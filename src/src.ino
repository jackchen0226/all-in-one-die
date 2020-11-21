#include <Arduino.h>
//#include <Tone32.h>   // Needed for ESP32, obtained here https://github.com/lbernstone/Tone32
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
//#include <elapsedMillis.h>
//#include <TrueRandom.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// J.C: We have 2 types of display connections, comment out the one you're NOT using

// I2C connected SSD1306 display
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*
// SPI connected SSD1306 display
#define OLED_MOSI   9	// Master output, Slave input (D1 on chip)
#define OLED_CLK   10	// Clock (D0 on chip)
#define OLED_DC    11	// Data/Command pin (DC on chip)
#define OLED_CS    12	// Chip select (CS on chip)
#define OLED_RESET 13	// Reset (RST on chip)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
*/


// Used for software SPI
#define LIS3DH_CLK 	13	// Clock (SCK on chip)
#define LIS3DH_MISO 12	// Master input, Slave output (SDO on chip)
#define LIS3DH_MOSI 11	// Master output, Slave input (SDA on chip)
// Used for hardware & software SPI
#define LIS3DH_CS 	10	// Chip select (CS on chip)
#define LIS3DH_INT	21	// Interrupt pin (INT on chip), disable if not in use -J.C
Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS, LIS3DH_MOSI, LIS3DH_MISO, LIS3DH_CLK);

// Adjust this number for the sensitivity of the 'click' force
// this strongly depend on the range! for 16G, try 5-10
// for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80
#define CLICKTHRESHHOLD 125

// These values are only used if LIS3DH_INT is used for rolling animation 
// ODR is defined with lis.setDataRate() -J.C
#define CLICKLIMIT	10	// Time in 1/ODR that acceleration occurs for a click
#define CLICKLATENCY 50	// Time in 1/ODR that the interrupt remains high 



// DEFINITIONS FOR POTENTIOMETER
#define ROLLd4 4
#define ROLLd6 6
#define ROLLd8 8
#define ROLLd10 10
#define ROLLd12 12
#define ROLLd20 20

const int SENSOR_PIN = A0;  //sensor for pot
int sensorValue = 0;


#define BUZZER_PIN 15	// Digital pin for buzzer control
int ROLL = 0;



void setup(void) {

	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	display.clearDisplay();
	display.display();


	#ifndef ESP8266
	  while (!Serial) yield();     // will pause Zero, Leonardo, etc until serial console opens
	#endif

	Serial.begin(9600);
	Serial.println("Adafruit LIS3DH Tap Test!");

	if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
		Serial.println("Couldnt start");
	while (1) yield();
	}
	Serial.println("LIS3DH found!");

	lis.setRange(LIS3DH_RANGE_2_G);   // 2, 4, 8 or 16 G!

	Serial.print("Range = "); Serial.print(2 << lis.getRange());  
	Serial.println("G");
	// 0 = turn off click detection & interrupt
	// 1 = single click only interrupt output
	// 2 = double click only interrupt output, detect single click
	// Adjust threshhold, higher numbers are less sensitive
	lis.setClick(1, CLICKTHRESHHOLD);
	delay(100); 

	pinMode(BUZZER_PIN, OUTPUT);
  
}


// Function to set the die number.
int selectRoll(){
  int result;  
  int test=0;
  int check = 0;
  
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);     // Start at top-left corner
  display.println("Enter the die number");
  display.display();


  while(test == 0){
    sensorValue = analogRead(SENSOR_PIN);
    sensorValue = map(sensorValue, 0, 1023, 0, 20);
    delay(1000);  // Delay to let person turn pot


  if(sensorValue <= ROLLd4){
    display.clearDisplay();

    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(45, 10);     // Start at top-left corner
    display.println("D4");
    display.setCursor(0,20);
    display.println("Keep dial still to roll");
    display.display();

    
    delay(1000);
    check = analogRead(SENSOR_PIN);
    check = map(check, 0, 1023, 0, 20);
    if(check == sensorValue){
    result = sensorValue + 1;  // add 1 to result for random number function call later
    test = 1;      
    }
    delay(900);
  }
  else if(sensorValue > ROLLd4 && sensorValue <= ROLLd6){
        
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(45, 10);     // Start at top-left corner
    display.println("D6");
    display.setCursor(0,20);
    display.println("Keep dial still to roll");
    display.display();
    
    delay(1000);
    check = analogRead(SENSOR_PIN);
    check = map(check, 0, 1023, 0, 20);
    if(check == sensorValue){
    result = sensorValue + 1;  // add 1 to result for random number function call later
    test = 1;      
    }
    delay(900);
  }
  else if(sensorValue > ROLLd6 && sensorValue <= ROLLd8){
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(45, 10);     // Start at top-left corner
    display.println("D8");
    display.setCursor(0,20);
    display.println("Keep dial still to roll");
    display.display();
    
    delay(1000);
    check = analogRead(SENSOR_PIN);
    check = map(check, 0, 1023, 0, 20);
    if(check == sensorValue){
    result = sensorValue + 1;  // add 1 to result for random number function call later
    test = 1;      
    }
    delay(900);
  }
  else if(sensorValue > ROLLd8 && sensorValue <= ROLLd10){
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(45, 10);     // Start at top-left corner
    display.println("D10");
    display.setCursor(0,20);
    display.println("Keep dial still to roll");
    display.display();
   
    delay(1000);
    check = analogRead(SENSOR_PIN);
    check = map(check, 0, 1023, 0, 20);
    if(check == sensorValue){
    result = sensorValue + 1;  // add 1 to result for random number function call later
    test = 1;      
    }
    delay(900);
  }
  else if(sensorValue > ROLLd10 && sensorValue <= ROLLd12){
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(45, 10);     // Start at top-left corner
    display.println("D12");
    display.setCursor(0,20);
    display.println("Keep dial still to roll");
    display.display();
    
    delay(1000);
    check = analogRead(SENSOR_PIN);
    check = map(check, 0, 1023, 0, 20);
    if(check == sensorValue){
    result = sensorValue + 1;  // add 1 to result for random number function call later
    test = 1;      
    }
    delay(900);
  }
  else{
        
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(45, 10);     // Start at top-left corner
    display.println("D20");
    display.setCursor(0,20);
    display.println("Keep dial still to roll");
    display.display();
    
    delay(1000);
    check = analogRead(SENSOR_PIN);
    check = map(check, 0, 1023, 0, 20);
    if(check == sensorValue){
    result = sensorValue + 1;  // add 1 to result for random number function call later
    test = 1;      
    }
    delay(900);
  }

  }
  delay(1000);
  return result;
  }



int diceNum;
int passRoll;
int check;
int flag = 1;
void loop() {

  if(flag == 1){
    diceNum = selectRoll();  // Call to function to select die value
    flag = 0;
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(15, 10);     // Start at top-left corner
    display.println("SHAKE SHAKE SHAKE");
    display.display();

    
    delay(1000);
  }
  
  uint8_t click = 0;
  click = lis.getClick();
  if (click == 0) return;
  if (! (click & 0x30)) return;  
  if(click & 0x10) { 

      ROLL = random(1, diceNum);
      display.clearDisplay();
      display.setTextSize(1);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(30, 10);     // Start at top-left corner
      display.println("YOU ROLLED A");
      display.setCursor(60, 20); 
      display.println(ROLL);
      display.display();
      delay(1000);
      if (ROLL == 1){
        tone(BUZZER_PIN, 200, 1000);
        delay(500);
        tone(BUZZER_PIN, 100, 1000);
        delay(500);
        tone(BUZZER_PIN, 80, 4000);
        delay(1000);
        noTone(BUZZER_PIN);
        delay(1000);
      }

    
      display.clearDisplay();
      display.setTextSize(1);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 10);     // Start at top-left corner
      display.println("Move dial to roll"); 
      display.setCursor(20,20);
      display.println("a different die");
      display.display();
      delay(4000);
      check = analogRead(SENSOR_PIN);
      check = map(check, 0, 1023, 0, 20)+1;
      
      

      if(diceNum == check){
        Serial.println("Shake again");
        display.clearDisplay();
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(30, 10);     // Start at top-left corner
        display.println("SHAKE AGAIN");
        display.display();
        flag = 0;
       
   
      }
      else if(diceNum != check){
        flag = 1;
}
return;
} }
