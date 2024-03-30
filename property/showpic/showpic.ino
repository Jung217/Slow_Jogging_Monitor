#include "TFT_eSPI.h"
#include "mini.h"
#include "hb.h"
#include "bb.h"
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
int a = 0;
const short unsigned int* arr[49] = {f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49};
void setup() {
  Serial.begin(115200);
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  sprite.createSprite(320, 170);
  sprite.setSwapBytes(true);

  //sprite.pushImage(10, 10, 180, 100, mini);
  /*sprite.pushImage(10, 10, 50, 50, hbt);
  sprite.pushSprite(0, 0);*/
}

void loop() {
  /*if(a==0) sprite.pushImage(10, 10, 50, 50, hb1);
  if(a==1) sprite.pushImage(10, 10, 50, 50, hb2);
  if(a==2) sprite.pushImage(10, 10, 50, 50, hb3);
  if(a==3) sprite.pushImage(10, 10, 50, 50, hb4);
  if(a==4) sprite.pushImage(10, 10, 50, 50, hb5);
  if(a==5) sprite.pushImage(10, 10, 50, 50, hb6);
  if(a==6) sprite.pushImage(10, 10, 50, 50, hb7);
  if(a==7) sprite.pushImage(10, 10, 50, 50, hb8);
  if(a==8){
    sprite.pushImage(10, 10, 50, 50, hb9);
    a=0;
  }
  else a++;
  delay(75);
  sprite.pushSprite(0, 0);*/
  int b=0;
  for(int i=0 ; i<97 ; i++){
    if(i<49) sprite.pushImage(10, 10, 50, 50, arr[i]);
    else {
      b+=2;
      sprite.pushImage(10, 10, 50, 50, arr[i-b]);
    }
    sprite.pushSprite(0, 0);
    delay(50);
  }
  sprite.setTextSize(4);
  sprite.setTextColor(TFT_WHITE);
  sprite.setCursor(60, 17);
  sprite.println(" BPM");
}
