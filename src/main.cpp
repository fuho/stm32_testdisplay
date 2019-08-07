/*
This is an example of how to use the OLED 128x64 I2C with SSD1306 driver using the Adafruit library.
It also applies to the 128x32 version, but not all components would fit the smaller screen.

Pins:
 * GND = GND
 * VCC = 5V
 * SCL = A5
 * SDA = A4

You can connect VCC to 3.3V to reduce the amount of high pitched noise that the display produces.

It's a good idea to put a resistor between A4-5V and A5-5V to help stabilize the connection.
What that does is pull-up the I2C pins to make it more reliable and prevents lock-ups.


Libraries needed:
https://github.com/adafruit/Adafruit_SSD1306
https://github.com/adafruit/Adafruit-GFX-Library


Guide for the GFX Library: https://cdn-learn.adafruit.com/downloads/pdf/adafruit-gfx-graphics-library.pdf

List of fonts: https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts

List of fonts that support right alignment:
FreeMono9pt7b.h
FreeMono12pt7b.h
FreeMono18pt7b.h
FreeMono24pt7b.h
FreeMonoBold9pt7b.h
FreeMonoBold12pt7b.h
FreeMonoBold18pt7b.h
FreeMonoBold24pt7b.h
FreeMonoBoldOblique9pt7b.h
FreeMonoBoldOblique12pt7b.h
FreeMonoBoldOblique18pt7b.h
FreeMonoBoldOblique24pt7b.h
FreeMonoOblique9pt7b.h
FreeMonoOblique12pt7b.h
FreeMonoOblique18pt7b.h
FreeMonoOblique24pt7b.h


This sketch was featured on my video tutorial for this display: https://www.youtube.com/watch?v=PrIAnDZ9dp8

Sketch made by: InterlinkKnight
Last modification: 10/05/2019
*/

#include <Adafruit_GFX.h>  // Include core graphics library for the display
#include <Adafruit_SSD1306.h>  // Include Adafruit_SSD1306 library to drive the display
#include <Fonts/FreeMonoBold12pt7b.h>  // Add a custom font
#include <Fonts/FreeMono9pt7b.h>  // Add a custom font
#include <AdvancedRotaryEncoder.h>

#define PIN_A   PB5 //ky-040 clk pin, add 100nF/0.1uF capacitors between pin & ground!!!
#define PIN_B   PB4 //ky-040 dt  pin, add 100nF/0.1uF capacitors between pin & ground!!!
#define BUTTON  PB3 //ky-040 sw  pin, add 100nF/0.1uF capacitors between pin & ground!!!


AdvancedRotaryEncoder encoder(PIN_A, PIN_B, BUTTON);
Adafruit_SSD1306 display(128, 64);  // Create display

int position = 0;
double angle = 0.0;
u_int lastChangeAt = millis();
u_int lastDeltaT = 0xFFFFFFFF;



void onEncoderEvent(EncoderEvent event){
    digitalWrite(LED_BUILTIN,LOW);
    delay(10);
    digitalWrite(LED_BUILTIN,HIGH);


    lastDeltaT = event.millis - lastChangeAt;
    lastChangeAt = event.millis;
    switch (event.eventType){
        case MoveCW:
        case MoveCCW:
            position = encoder.getPosition();
            angle = encoder.getAngle();
            break;
        case ButtonDown:
            digitalWrite(LED_BUILTIN, LOW);
            encoder.setPosition(0);
            break;
        case ButtonUp:
            digitalWrite(LED_BUILTIN, HIGH);
            encoder.setAngle(0);
            break;
        case None:
            break;
    }

}

void setup(){
    //Led
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN,HIGH);

    // Encoder
    encoder.setListener(onEncoderEvent);
    encoder.start();

    //Display
    delay(50);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize display with the I2C address of 0x3C
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setRotation(0);
    display.setTextWrap(false);
    display.dim(false);
    display.setFont(&FreeMonoBold12pt7b);
    display.setTextSize(0);  // Always zero for custom font
}

void loop() {
    display.clearDisplay();

    char angleString[10];
    dtostrf(-18 * position, 3, 0, angleString);
    display.setCursor(5, random(20, 22));
    display.println(angleString);

    char deltaString[10];
    dtostrf(lastDeltaT, 3, 0, deltaString);
    display.setCursor(5, random(50, 52));
    display.println(deltaString);

    display.display();
}
