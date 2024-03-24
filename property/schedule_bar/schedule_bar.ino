#include <Arduino.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
 
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
 
void fillSegment(int x,int y,unsigned int colour)
{
  int y2 = 10;
  for (int i=0 ; i<+300 ; i++)
  {
 
    tft.fillRect(x, y,  i,  y2, colour);
    //如果在这里加个delay会出现扇形动画
    delay(20);
  }
}
 
void setup(void)
{
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
 
}
 
void loop()
{
  fillSegment(10, 85, TFT_WHITE);
  tft.fillScreen(TFT_BLACK);
 
}
 
