#include "TFT_eSPI.h"
#include "mini.h"
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

void setup() {
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  sprite.createSprite(320, 170);
  sprite.setSwapBytes(true);

  sprite.pushImage(10, 10, 180, 150, mini);
  sprite.pushSprite(0, 0);
}

void loop() {

}
