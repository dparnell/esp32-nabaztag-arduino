#include <Arduino.h>
#include <NeoPixelBus.h>

extern "C" {
#include"vmem.h"
#include"vloader.h"
#include"vinterp.h"
#include"properties.h"
#include "log.h"
#include "linux_simunet.h"

//void simuSetLed(int i,int val);
void simuSetMotor(int i,int val);

}

#include "bc.h"

#define BUTTON_PIN 15
#define PIXELS 5
#define PIXEL_PIN 4
NeoPixelBus<NeoGrbFeature, NeoWs2813Method> strip(PIXELS, PIXEL_PIN);

#define ADJUST_BRIGHTNESS(v) ((v) / 4)

extern "C" void simuSetLed(int i,int val) {
  RgbColor c(ADJUST_BRIGHTNESS((val>>16)&255), ADJUST_BRIGHTNESS((val>>8)&255), ADJUST_BRIGHTNESS((val)&255));
  strip.SetPixelColor(i, c);
}

extern "C" int getButton() {
  return digitalRead(BUTTON_PIN);
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(PIXEL_PIN, OUTPUT);

  strip.Begin();
  strip.Show();


  loaderInit((char*)dumpbc);

  VPUSH(INTTOVAL(0));
  interpGo();
  VPULL();
}

void loop() {
  checkNetworkEvents();

	VPUSH(VCALLSTACKGET(sys_start,SYS_CBLOOP));
	if (VSTACKGET(0)!=NIL) interpGo();
	VPULL();

  // update the LEDs
  strip.Show();
}
