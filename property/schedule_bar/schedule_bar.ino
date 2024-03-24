#include <Arduino.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

 
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
 
static const unsigned char PROGMEM O2_bmp[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x3f, 0xc3, 0xf8, 0x00, 0xff, 0xf3, 0xfc,
  0x03, 0xff, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xfe, 0x0f, 0xff, 0xff, 0xfe, 0x0f, 0xff, 0xff, 0x7e,
  0x1f, 0x80, 0xff, 0xfc, 0x1f, 0x00, 0x7f, 0xb8, 0x3e, 0x3e, 0x3f, 0xb0, 0x3e, 0x3f, 0x3f, 0xc0,
  0x3e, 0x3f, 0x1f, 0xc0, 0x3e, 0x3f, 0x1f, 0xc0, 0x3e, 0x3f, 0x1f, 0xc0, 0x3e, 0x3e, 0x2f, 0xc0,
  0x3e, 0x3f, 0x0f, 0x80, 0x1f, 0x1c, 0x2f, 0x80, 0x1f, 0x80, 0xcf, 0x80, 0x1f, 0xe3, 0x9f, 0x00,
  0x0f, 0xff, 0x3f, 0x00, 0x07, 0xfe, 0xfe, 0x00, 0x0b, 0xfe, 0x0c, 0x00, 0x1d, 0xff, 0xf8, 0x00,
  0x1e, 0xff, 0xe0, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00,
  0x0f, 0xe0, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void fillSegment(int x,int y,unsigned int colour) //  tft.fillScreen(TFT_BLACK);
{
  int y2 = 10;
  for (int i=0 ; i<+300 ; i++)
  {
    tft.fillRect(x, y,  i,  y2, colour);
    delay(20);
  }
}

void introView()
{
  tft.fillScreen(TFT_BLACK);
  delay(100);
  for(int i=170 ; i>=0 ; i--)
  {
    tft.drawPixel(30, i, TFT_ORANGE);
    tft.drawPixel(31, i, TFT_ORANGE);
    tft.drawPixel(32, i, TFT_ORANGE);
    delay(2.5);
  }
  for(int i=0 ; i<=320 ; i++)
  {
    tft.drawPixel(i, 149, TFT_ORANGE);
    tft.drawPixel(i, 150, TFT_ORANGE);
    tft.drawPixel(i, 151, TFT_ORANGE);
    delay(2.5);
  }

  delay(150);
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(5);
  tft.setCursor(50, 10);
  tft.print("Slow");
  delay(200);
  tft.setCursor(50, 56);
  tft.print("Jogging");
  delay(100);
  tft.setCursor(50, 102);
  tft.print("Monitor");
  delay(150);
  tft.setTextSize(1);
  tft.setCursor(50, 160);
  tft.print("Ver.1.0.1"); // version

  delay(1500);

  tft.fillScreen(TFT_BLACK);
}
void setup(void)
{
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  delay(500);
  introView();
}
 
void loop()
{
}
 
