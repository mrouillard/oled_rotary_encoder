#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <Fonts/FreeSans12pt7b.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define OLED_ADDR 0x3C

#define encoderPinA 2
#define encoderPinB 3
#define encoderButton 4

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define DEBOUNCE_TIME 10

// Keep track of last rotary value
long lastRotaryPosition = 1;
long clickedValue = 0;

// Updated by the ISR (Interrupt Service Routine)
volatile long virtualPosition = 0;

void setup()   {                
  Serial.begin(9600);
  Serial.println("Ok!");
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(encoderButton, INPUT_PULLUP);
  // init done

  attachInterrupt(digitalPinToInterrupt(encoderPinA), interruptManagePosition, RISING); //third param can be LOW, RISING, CHANGE, FALLING, HIGH
  
  // Clear the buffer.
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.clearDisplay();
  display.display();
}

void loop() {
  // Is someone pressing the rotary encoder switch?
  if ((!digitalRead(encoderButton))) {
    clickedValue = virtualPosition;
    while (!digitalRead(encoderButton))
      delay(DEBOUNCE_TIME);
    Serial.println("Click!");
    tone(8, 440, 50);
    showCount(virtualPosition,clickedValue);
  }
  // Is someone rotating the rotary encoder?
  if(lastRotaryPosition != virtualPosition) {
    showCount(virtualPosition,clickedValue);
    lastRotaryPosition = virtualPosition;
  }
}

void showCount(long i, long j) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Niveau:");
  display.setCursor(63,0);
  display.print(i);
  display.setCursor(0,12);
  display.print("Val. clic:");
  display.setCursor(90,12);
  display.print(j);
  display.display();
}

void interruptManagePosition() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // If interrupts come faster than 5ms, assume it's a bounce and ignore
  if (interruptTime - lastInterruptTime > 5) {
    if(digitalRead(encoderPinB) == digitalRead(encoderPinA)) {
      virtualPosition--;
    } else {
      virtualPosition++;
    }
  }
  // Keep track of when we were here last (no more than every 5ms)
  lastInterruptTime = interruptTime;
}
