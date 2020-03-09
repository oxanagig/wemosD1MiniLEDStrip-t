#include <Homie.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>

#define FW_NAME "ledstrip-control"
#define FW_VERSION "0.1.0"

#define LED_PIN            D1
#define LED_COUNT      30


int delayval = 50; 
int Hue;
int Saturation;
int Brightness;
int numPixels = LED_COUNT;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Ticker LEDTimer;

HomieNode ledNode("led", "led","ledStrip");

void colorWipe(uint32_t c, uint8_t wait) {
  int i;
 
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void ledHandler(){
  static uint8_t alternate = 1;

  alternate^=1; 
  if(alternate == 1)
  {
    uint32_t newColor = random(0xFFFFFFFF);
    colorWipe(newColor,delayval);
  }
  else
  {
    colorWipe(0, delayval);
  }

}



bool lightOnHandler(const HomieRange& range, const String& value) {

  Homie.getLogger()<< "the color is "<<value<<endl;

  LEDTimer.detach();

  if (value == "off") {
    colorWipe(strip.Color(0, 0, 0), delayval);    // Black/off
    ledNode.setProperty("color").send("0,0,0");
  } else if (value == "red") {
    colorWipe(strip.Color(255, 0, 0), delayval);  // Red
    //ledNode.setProperty("color").send("255,0,0");
  } else if (value == "green") {
    colorWipe(strip.Color(0, 255, 0), delayval);  // Green
    //ledNode.setProperty("color").send("0,255,0");
  } else if (value == "blue") {
    colorWipe(strip.Color(0, 0, 255), delayval);  // Blue
    //ledNode.setProperty("color").send("0,0,255");
  } else if(value == "nightClub"){
    LEDTimer.attach_ms(100,ledHandler);
  } else {
      Hue = value.substring(0,value.indexOf(',')).toInt();
      Hue = ((Hue*255)/360)<<8;
      Saturation = value.substring(value.indexOf(',')+1,value.lastIndexOf(',')).toInt();
      Saturation = (Saturation*255)/100;
      Brightness = value.substring(value.lastIndexOf(',')+1).toInt();
      Brightness = (Brightness*255)/100;
      colorWipe(strip.ColorHSV(Hue, Saturation,Brightness), delayval);
      String newColor = String(Hue) +','+String(Saturation)+','+String(Brightness);
      ledNode.setProperty("color").send(value);
  }
  return true;
}

void setup() {
  /* Setup Random seed for the party light mode */
  randomSeed(analogRead(0));

  Serial.begin(115200);
  Serial << endl << endl;

  pinMode(LED_PIN, OUTPUT);
  strip.begin();

  Homie_setFirmware(FW_NAME,FW_VERSION);

  ledNode.setProperty("color").send("0,0,0");
  ledNode.advertise("color").settable(lightOnHandler);


  Homie.setup();
}

void loop() {
  Homie.loop();
}