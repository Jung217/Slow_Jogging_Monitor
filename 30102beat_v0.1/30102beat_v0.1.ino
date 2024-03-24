#include "MAX30105.h"
#include "heartRate.h"      //checkforbeat
#include "TFT_eSPI.h"

#define btn 14
#define FINGER_ON 7000      //紅外線最小量（判斷手指有沒有上）
#define min_SPO2 90.0       //血氧最小量
#define beep 1
MAX30105 pox;
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

//Beep
int mode = 0;
float modeArr[4] = {10000, 353.36, 333.3, 316.46};

//Beat
const byte RATE_SIZE = 50;   //多少平均數量
byte rates[RATE_SIZE];      //心跳陣列
byte rateSpot = 0;
long lastBeat = 0;          //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

//Spo2
double avered = 0;
double aveir = 0;
double sumirrms = 0;
double sumredrms = 0;

double SpO2 = 0;
double ESpO2 = 90.0;      //初始值
double FSpO2 = 0.7;       //filter factor for estimated SpO2
double frate = 0.95;      //low pass filter for IR/red LED value to eliminate AC component
int i = 0;
int Num = 30;             //取樣30次才計算1次

void tftinit(){
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  tft.init();
  tft.setRotation(3);

  sprite.createSprite(320, 170);
  sprite.setSwapBytes(true);
}

void sensorSetup(){
  byte ledBrightness = 0x7F;    //亮度建議=127, Options: 0=Off to 255=50mA
  byte sampleAverage = 4;       //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2;             //Options: 1 = Red only(心跳), 2 = Red + IR(血氧)
  int sampleRate = 800;         //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 215;         //Options: 69, 118, 215, 411
  int adcRange = 16384;         //Options: 2048, 4096, 8192, 16384
  
  pox.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
  pox.enableDIETEMPRDY();

  pox.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  pox.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void task30102(){
  long irValue = pox.getIR();

  if (irValue > FINGER_ON) {
    if (checkForBeat(irValue) == true) {  //檢查是否有心跳，測量心跳
      long delta = millis() - lastBeat;//計算心跳差
      lastBeat = millis();
      beatsPerMinute = 60 / (delta / 1000.0);
      if (beatsPerMinute > 20 && beatsPerMinute < 255) {  //心跳必須再20-255之間
        rates[rateSpot++] = (byte)beatsPerMinute; //儲存心跳數值陣列
        rateSpot %= RATE_SIZE;
        beatAvg = 0;//計算平均值
        for (byte x=0 ; x<RATE_SIZE ; x++) beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }

    uint32_t ir, red;  //測量血氧
    double fred, fir;
    pox.check(); //Check the sensor, read up to 3 samples
    if (pox.available()) {
      i++;
      ir = (double)pox.getFIFOIR(); //讀取紅外線
      red = (double)pox.getFIFORed(); //讀取紅光

      aveir = aveir * frate + ir * (1.0 - frate);     //average IR level by low pass filter
      avered = avered * frate + red * (1.0 - frate);  //average red level by low pass filter
      sumirrms += (ir - aveir) * (ir - aveir);        //square sum of alternate component of IR level
      sumredrms += (red - avered) * (red - avered);   //square sum of alternate component of red level

      if ((i % Num) == 0) {
        double R = (sqrt(sumirrms) / aveir) / (sqrt(sumredrms) / avered);
        SpO2 = -23.3 * (R - 0.4) + 100;
        ESpO2 = FSpO2 * ESpO2 + (1.0 - FSpO2) * SpO2; //low pass filter
        if (ESpO2 <= min_SPO2) ESpO2 = min_SPO2;      //indicator for finger detached
        if (ESpO2 > 100) ESpO2 = 99.9;
        sumredrms = 0.0; sumirrms = 0.0; SpO2 = 0;
        i = 0;
      }
      pox.nextSample();
    }
    Serial.print("Bpm:" + String(beatAvg));
    Serial.println(",SPO2:" + String(ESpO2));
  }
  else {  //清除數據
    Serial.println("Finger not found");
    for (byte rx = 0 ; rx < RATE_SIZE ; rx++) rates[rx] = 0;
    beatAvg = 0; rateSpot = 0; lastBeat = 0;
    
    avered = 0; aveir = 0; sumirrms = 0; sumredrms = 0;
    SpO2 = 0; ESpO2 = 90.0;
  }
}

void beepHz(int val)
{
  if(!val && ++mode == 4) mode = 0; 
  if(mode != 0)
  {
    digitalWrite(beep, HIGH);
    delay(10); 
    digitalWrite(beep, LOW);
    delay(modeArr[mode]-10);
  } 
  else delay(500);
}

void setup() {
  Serial.begin(115200);
  pinMode(beep, OUTPUT);
  pinMode(btn, INPUT);
  
  xTaskCreatePinnedToCore (
    loop2,     // Function to implement the task
    "loop2",   // Name of the task
    1000,      // Stack size in words
    NULL,      // Task input parameter
    0,         // Priority of the task
    NULL,      // Task handle.
    0          // Core where the task should run
  );  
  Serial.println("System Start");
  if (!pox.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("Sensor not found");
    while (1);
  }
  sensorSetup();
}

void loop() {
  task30102();
}
void loop2 (void* pvParameters) {
  while(1){
    int val = digitalRead(btn);
    beepHz(val);
  }
}