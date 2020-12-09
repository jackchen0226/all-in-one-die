#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// _______________DISPLAY________________
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

#define CHAR_HEIGHT 8     // Height of character in standard font
#define CHAR_WIDTH  6     // Width of the character in standard font

// J.C: We have 2 types of display connections, comment out the one you're NOT using
// I2C connected SSD1306 display
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_ADDR 0x3C   // Address of the OLED display
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


// ________________ACCELEROMETER________________
// Used for software SPI
#define LIS3DH_CLK 	5	// Clock (SCK on chip)
#define LIS3DH_MISO 19	// Master input, Slave output (SDO on chip)
#define LIS3DH_MOSI 18	// Master output, Slave input (SDA on chip)
// Used for hardware & software SPI
#define LIS3DH_CS 	32	// Chip select (CS on chip)
#define LIS3DH_INT	21	// Interrupt pin (INT on chip), disable if not in use -J.C
Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS, LIS3DH_MOSI, LIS3DH_MISO, LIS3DH_CLK);

// Adjust this number for the sensitivity of the 'click' force
// this strongly depend on the range! for 16G, try 5-10
// for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80
#define CLICKTHRESHHOLD 125

// These values are only used if LIS3DH_INT is used for rolling animation 
// ODR is defined with lis.setDataRate() in setup() -J.C
#define CLICKLIMIT	13	// Time in 1/ODR that acceleration must be maintained for a click
#define CLICKLATENCY 50	// Time in 1/ODR that the interrupt remains high 


// ____________POTENTIOMETER_____________
// DEFINITIONS FOR POTENTIOMETER
#define ROLLd4 4
#define ROLLd6 6
#define ROLLd8 8
#define ROLLd10 10
#define ROLLd12 12
#define ROLLd20 20

const int SENSOR_PIN = A0;  //sensor for pot
int sensorValue = 0;
#define ADC_MAX 4096        // maximum analog read value from ADC, 4096 for ESP32, 1024 for Uno/Atmega


// _____________BUZZER_________________
#define BUZZER_PIN 15	// Digital pin for buzzer control
// Used for the esp32 implementation of the buzzer, since tone() does not work -J.C
#define BUZZER_FREQ 2000  // PWM freq
#define BUZZER_CH 0       // Digital channel of the buzzer
#define RES 8             // Resolution


// ______________MISC_________________
int ROLL = 0;
#define USE_ESP32 true    // CHANGE IF BOARD IS ESP32 -J.C



void setup(void) {
  Serial.begin(9600);
  #ifndef ESP8266
    while (!Serial) yield();     // will pause Zero, Leonardo, etc until serial console opens
  #endif
  
  // ___________DISPLAY___________
  // Set up display at I2C address OLED_ADDR
	display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
	display.clearDisplay();
	display.display();


  // ___________ACCELEROMETER__________
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
	//lis.setClick(1, CLICKTHRESHHOLD);

  // Alternative click using interrupt pins
  // Manually setting data rate of 25 Hz (40 ms, less than a fast reaction of 120 ms)
  lis.setDataRate(LIS3DH_DATARATE_25_HZ);
  lis.setClick(1, CLICKTHRESHHOLD, CLICKLIMIT, CLICKLATENCY);
	delay(100); 

  // ____________BUZZER___________
  if (USE_ESP32) {
    // Set up led PWM to be used for a buzzer
	  ledcSetup(BUZZER_CH, BUZZER_FREQ, RES);
    // Attach digital channel to physical pin
    ledcAttachPin(BUZZER_PIN, BUZZER_CH);
    ledcWrite(BUZZER_CH, 125);  // Set to 50% duty cycle (half of 2^RES)
    ledcWriteTone(BUZZER_CH, LOW);

  }
  else {
    pinMode(BUZZER_PIN, OUTPUT);
  }
  
  //____________POTENTIOMETER AND RANDOMNESS___________
  // Randomness would be a generated value 
  // We use 3 sources of real-world randomness
  // 1) The fluctuations of an analog pin
  // 2) The x position of the accelerometer
  // 3) The y acceleration of the accelerometer
  //
  // We use x pos because testing had a fluctuation range of ~150
  // y acceleration chosen arbitrarily
  // analog pin to add another source of randomness
  // -J.C

  int pot_value = analogRead(SENSOR_PIN);
  lis.read();   // Gets X Y and Z data at once

  // Get the normalized acceleration event
  sensors_event_t event;
  lis.getEvent(&event);

  // Seed RNG
  // lis.x and event.acceleration.y can go negative, causing overflows
  // making this function more chaotic. Good for randomness -J.C.
  randomSeed((unsigned int)(lis.x * event.acceleration.y - pot_value));

}


void write_out(int curr_state, bool shaking, int roll) {
  // boilerplate, clear out display and set text
  display.clearDisplay();
  display.setTextSize(1);   // Font size to 2, characters are 12x16 pixels
  display.setTextColor(SSD1306_WHITE);  // Doesn't matter for most displays, can have 2 args for bg
  display.cp437(true);      // Set font as 'Code Page 437'

  display.setCursor(0, 0);

  // Display out current state
  // While currently each case has few differences, the following
  // should be kept in switch() statement for future implementation -J.C
  switch(curr_state) {
    case 1:
      display.println(F("CURRENT DICE:  D 1"));
      break;
    case ROLLd4:
      display.println(F("CURRENT DICE:  D 4"));
      break;
    case ROLLd6:
      display.println(F("CURRENT DICE:  D 6"));
      break;
    case ROLLd8:
      display.println(F("CURRENT DICE:  D 8"));
      break;
    case ROLLd10:
      display.println(F("CURRENT DICE:  D10"));
      break;
    case ROLLd12:
      display.println(F("CURRENT DICE:  D12"));
      break;
    case ROLLd20:
      display.println(F("CURRENT DICE:  D20"));
      break;
    default:
      display.write("CURRENT DICE: ");
      if (roll >= 100)
        display.write('D');
      else
        display.write(" D");
      display.write(curr_state / 100 + '0');        // Get 100s digital
      display.write((curr_state / 10) % 10  + '0'); // Get 10s digit (e.g. 323 / 10 == 32. 32 % 10 == 2
      display.write((curr_state % 10) + '0');
  }

  // Display if currently shaking
  if (shaking) {
    display.setTextSize(2);

    
    // Write to row 1
    display.setCursor(0, CHAR_HEIGHT*1);
    display.println(F("SHAKING!!!"));             // Size 2 font has 10 char max

    display.setTextSize(1);
    // Write to row 3
    display.setCursor(0, CHAR_HEIGHT*3);
    display.println(F(" ROLLING!  ROLLING! "));    // Size 1 font has 21 char max
 
    // Display the buffer, then begin scrolling display
    display.display();
    display.startscrollright(0x01, 0x0F);

    // Premature return to prevent double display, just in case.
    return;
  }
  else if (roll >= 0) {
    // Make sure to stop scrolling once roll is resolved
    display.stopscroll();

    display.setTextSize(2);
    display.setCursor(0, CHAR_HEIGHT*1);    // 5px offset centers 9, 2x chars
    display.write("ROLLED: ");

    /*
    if (roll > 99)
      display.setCursor(5, CHAR_HEIGHT*3);    // 5px offset centers 3, 3x chars
    else
      display.setCursor(14, CHAR_HEIGHT*3);   // 14 ps offset centers 2, 3X chars
    */
    if (roll >= 100)
      display.write(roll / 100 + '0');      // Get 100s digit
    // No support for rolls past 1000, need for expandability? -J.C.
    if (roll >= 10)
      display.write((roll / 10) % 10  + '0'); // Get 10s digit (e.g. 323 / 10 == 32. 32 % 10 == 2
    display.write((roll % 10) + '0');
  }
  else {
    display.setTextSize(1);
    display.setCursor(0, CHAR_HEIGHT*2);

    display.println(F("Shake die for 0.5s"));
  }

  display.display();

}


// Comment out if using ESP32 -J.C
/*
void play_sound() {
  if (USE_ESP32) {
    play_sound_esp32()
  }
  else {
    // Megalovania
    // Composed by Toby Fox
    // D3 eigth note (beamed)
    tone(BUZZER_PIN, 147);
    delay(63);
    noTone(BUZZER_PIN);
    delay(63);
    
    // D3 eigth note (beamed)
    tone(BUZZER_PIN, 147);
    delay(63);
    noTone(BUZZER_PIN);
    delay(63);
  
    // D4 eigth note, dotted rest (beamed)
    tone(BUZZER_PIN, 294);
    delay(125);
    noTone(BUZZER_PIN);
    delay(125);
  
    // A4 quarter note, dotted rest/end
    tone(BUZZER_PIN, 220);
    delay(250);
    noTone(BUZZER_PIN);
  }
}*/

void play_sound_esp32() {
  // Megalovania
  // Composed by Toby Fox
  // D3 eighth note (beamed)
  ledcWriteTone(BUZZER_CH, 147);
  delay(63);
  ledcWriteTone(BUZZER_CH, LOW);
  delay(63);

  // D3 eighth note (beamed)
  ledcWriteTone(BUZZER_CH, 147);
  delay(63);
  ledcWriteTone(BUZZER_CH, LOW);
  delay(63);

  // D4 eigth note, dotted rest (beamed)
  ledcWriteTone(BUZZER_CH, 294);
  delay(125);
  ledcWriteTone(BUZZER_CH, LOW);
  delay(125);

  // A4 quarter note, dotted rest/end
  ledcWriteTone(BUZZER_CH, 220);
  delay(250);
  ledcWriteTone(BUZZER_CH, LOW);
}

// Variables deprecated -J.C
int diceNum;
int passRoll;
int check;
int flag = 1;

bool curr_shaking, prev_shaking;
bool display_shake;
int curr_dice_state;
void loop() {
  // Get and map a value from potentiometer
  // Currently it is just a linearly 6 binned map, can be adjusted -J.C.
  // 0 -> d4
  // 1 -> d6
  // 2 -> d8
  // 3 -> d10
  // 4 -> d12
  // 5 -> d20
  sensorValue = analogRead(SENSOR_PIN);
  sensorValue = map(sensorValue, 0, ADC_MAX, 0, 6);

  switch (sensorValue) {
    case 0:
      curr_dice_state = 1;
      break;
    case 1:
      curr_dice_state = ROLLd4;
      break;
    case 2:
      curr_dice_state = ROLLd6;
      break;
    case 3:
      curr_dice_state = ROLLd8;
      break;
    case 4:
      curr_dice_state = ROLLd10;
      break;
    case 5:
      curr_dice_state = ROLLd12;
      break;
    case 6:
      curr_dice_state = ROLLd20;
      break;
    default:
      curr_dice_state = 100;
      break;
  }

  // Get interrupt from accelerometer
  uint8_t click = 0;
  click = lis.getClick();

  // Boilerplate
  if (!(click & 0x30)) {
    curr_shaking = false;
    // No roll occurred, check if we shook last check
    
    if (prev_shaking) {
      // When we have a previous shake, but not current shake
      // signifies we've finished shake and ready to show roll
      // click transitioned from 1 -> 0
      ROLL = random(1, curr_dice_state);
      display_shake = false;

    }
    else {
      // If not, then we have not rolled anything
      // Display out that we're not shaking
      ROLL = -1;
      display_shake = false;
    }

  }
  else if (click & 0x10) {
    curr_shaking = true;
    // Roll occurred, check if we shook the last check
    if (prev_shaking) {
      // previously was shaking, so keep shaking
      ROLL = -1;
      display_shake = true;
    }
    else {
      // previous not shaking, so this must be the first shaking
      ROLL = -1;
      display_shake = true;
    }
  }

  write_out(curr_dice_state, display_shake, ROLL);
  // If we did display out the roll, we should wait and let it linger for a few
  // Roll only ever displayed when we previously shook and current not shaking
  if (prev_shaking & !curr_shaking) {
    if (ROLL == 1)
      play_sound_esp32();
    delay(5000);
  }

  // Add short delay.
  // Ideally this would be a sleep to reduce power consumption -J.C.
  delay(200);
  // At the end of loop, make curr_shaking to prev_shaking
  prev_shaking = curr_shaking;
}
