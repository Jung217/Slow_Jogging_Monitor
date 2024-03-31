#include "config.h"
#include "MAX30105.h"
#include "heartRate.h"      //checkforbeat
#include "SPIFFS.h"
#include "bb.h"
#include "bg.h"
#include "hb.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFi.h>

#define btn 14
#define FINGER_ON 7000      //紅外線最小量（判斷手指有沒有上）
#define min_SPO2 90.0       //血氧最小量
#define beep 1
MAX30105 pox;
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

const short unsigned int* arrH[9] = {hb1, hb2, hb3, hb4, hb5, hb6, hb7, hb8, hb9};
const short unsigned int* arrB[49] = {f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49};

const char* ssid = "JUNGDLINK";
const char* password = "jung0217";

//Beat
const byte RATE_SIZE = 100;   //多少平均數量
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
  /*pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);*/

  tft.init();
  tft.setRotation(3);

  sprite.createSprite(320, 170);
  sprite.setSwapBytes(true);

  tft.fillScreen(TFT_BLACK);
}

void fillSegment(int x,int y,unsigned int colour){ //  tft.fillScreen(TFT_BLACK); 
  int y2 = 10;
  int Delay = 20; 
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, 100);
  tft.print("Final Checking..."); //tft.drawCentreString(String("Final Checking..."), 40, 80);
  for (int i=0 ; i<=300 ; i++)
  {
    tft.fillRect(x, y,  i,  y2, colour);
    delay(5);
  }
  tft.print(" DONE!");
}

void introView(){
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
  tft.setCursor(50, 157);
  tft.print("Ver.2.1.3"); // version

  delay(1500);

  tft.fillScreen(TFT_BLACK);
}
void loading(){
  sensorSetup();
  initWiFi();
  fillSegment(10, 150, TFT_ORANGE);
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    //tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(20, 55);
    tft.print("Wi-Fi connecting...");
    delay(1000);
    if(millis() > 6L*1000L) break;
  }
  if(WiFi.status() == WL_CONNECTED) tft.print("  :)");
  else tft.print("  :(");
}

void sensorSetup(){
  byte ledBrightness = 0x7F;    //亮度建議=127, Options: 0=Off to 255=50mA
  byte sampleAverage = 4;       //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2;             //Options: 1 = Red only(心跳), 2 = Red + IR(血氧)
  int sampleRate = 800;         //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 215;         //Options: 69, 118, 215, 411
  int adcRange = 16384;         //Options: 2048, 4096, 8192, 16384
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, 10);
  tft.print("Sensor Setting...");
  pox.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
  pox.enableDIETEMPRDY();

  pox.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  pox.setPulseAmplitudeGreen(0); //Turn off Green LED

  tft.print("    :)");
}

int fili = 0 ;  
bool fingerOn=false;
void task30102(){
  long irValue = pox.getIR();
  
  if (irValue > FINGER_ON) {
    fingerOn=true;
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
    /*Serial.print("Bpm:" + String(beatAvg));
    Serial.println(",SPO2:" + String(ESpO2));*/
  }
  else {  //清除數據
    fingerOn=false;
    for (byte rx = 0 ; rx < RATE_SIZE ; rx++) rates[rx] = 0;
    beatAvg = 0; rateSpot = 0; lastBeat = 0;
    
    avered = 0; aveir = 0; sumirrms = 0; sumredrms = 0;
    SpO2 = 0; ESpO2 = 90.0;
  }
}

unsigned long runTime, startTime;
int Tsec, Tmin, Thr;
void calTime(){
  Tsec = Tmin = Thr = 0.0; 
  
  runTime = millis() - startTime;
  Tsec = runTime / 1000 % 60;
  Tmin = runTime / 1000 / 60 % 60;
  Thr = runTime / 1000 / 60 / 60 % 60;  
}

int mode = 0;
bool dataUp = false;
void beepHz(int chk){
  if(chk) mode++; 
  if(mode == 2) {
    mode = 0;
    dataUp = true;
  } 
  if(mode != 0)
  {
    digitalWrite(beep, HIGH);
    delay(10); 
    digitalWrite(beep, LOW);
    delay(333.3);
  }
}
int i1=0;
int i2=0;
int b=0;
void showdata(){
  //sprite.fillSprite(TFT_BLACK);
  sprite.pushImage(0, 0, 320, 170, bg);

  sprite.setTextColor(TFT_WHITE);
  sprite.setTextSize(3);
  sprite.setCursor(10, 135);

  if(!mode) sprite.print("T:00:00:00");
  else {
    calTime();
    sprite.print("T:");  //回傳時間
    if(Thr<10) {
      sprite.print("0"); 
      sprite.print(Thr);
    }
    else sprite.print(Thr); 
    sprite.print(":");
    if(Tmin<10) {
      sprite.print("0"); 
      sprite.print(Tmin); 
    }
    else sprite.print(Tmin);    
    sprite.print(":"); 
    if(Tsec<10) {
      sprite.print("0"); 
      sprite.print(Tsec);
    }
    else sprite.print(Tsec);
  }

  if (fingerOn){
    sprite.pushImage(10, 10, 50, 50, arrH[i1]);

    sprite.setTextSize(4);
    sprite.setCursor(75, 20);
    sprite.print(beatAvg); 
    sprite.println(".BPM");

    if(i2<49) sprite.pushImage(10, 70, 50, 50, arrB[i2]);
    else {
      b+=2;
      sprite.pushImage(10, 70, 50, 50, arrB[i2-b]);
    }
    sprite.setCursor(75, 80);
    if (beatAvg > 10) sprite.print(String(ESpO2) + "%");
    else sprite.print("--- %" );
    i1++;
    i2++;
    if(i1>8) i1 = 0;
    if(i2 >= 97) {
      i2 = 0;
      b = 0;
    }  
  }
  else{
    sprite.fillSprite(TFT_BLACK);
    sprite.setTextSize(3);
    sprite.setTextColor(TFT_RED);
    sprite.setCursor(35, 70);
    sprite.print("Hand not found");
  }
  sprite.pushSprite(0, 0);
}

/*void dbdata(void){ // Data2DB
  //tft.fillScreen(TFT_BLACK);
  if(WiFi.status()== WL_CONNECTED){
    char msg1[5] ="90";
    char msg2[5] ="60";
    char msg3[5] ="300";
    int AA = ESpO2;
    int BB = beatAvg;
    int CC = random(300, 1000);
    itoa(AA, msg1, 10);  
    itoa(BB, msg2, 10);
    itoa(CC, msg3, 10); 
    //tft.setTextColor(TFT_WHITE);
    //tft.drawString(String("Data uploading..."), 13, 55, 4);  
    http_update(msg1, msg2, msg3);
  }
  /*else {
    tft.setTextColor(TFT_RED);
    tft.drawString(String("Uploading Failed."), 13, 55, 4);
  }
  lastConnectTime = millis();
}*/

/*void http_update(char *msg1, char *msg2, char *msg3) {
  HTTPClient http;
  http.begin(EndPoint);
  http.addHeader("Content-Type", "application/json");
  String spo2 = "{\"spo2\": ";
  String beat = ", \"beat\": ";
  String mood = ", \"mood\": ";
  String pw = ", \"password\":";
  spo2.concat(msg1);
  beat.concat(msg2);
  mood.concat(msg3);
  pw.concat(pd);
  pw.concat("}");
  spo2.concat(beat.c_str());
  spo2.concat(mood.c_str());
  spo2.concat(pw.c_str());
  Serial.println(spo2);
  int httpResponseCode = http.POST(spo2);          
  /*if (httpResponseCode > 0) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Uploading Successful", 13, 55, 4);
  }
  else {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED);
    tft.drawString("Uploading Failed", 13, 55, 4);
  }
  http.end();
  delay(250);
}*/

bool start = false;
void setup() {
  Serial.begin(115200);
  pinMode(beep, OUTPUT);
  pinMode(btn, INPUT);
  tftinit();
  if (!pox.begin(Wire, I2C_SPEED_FAST))
  {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(40, 80);
    tft.print("Sensor Not Found :(");
    while (1);
  }
  xTaskCreatePinnedToCore (
    loop2,     // Function to implement the task
    "loop2",   // Name of the task
    8192,      // Stack size in words
    NULL,      // Task input parameter
    0,         // Priority of the task
    NULL,      // Task handle.
    0          // Core where the task should run
  );  
  delay(500);
  introView();
  loading();
  delay(250);
  start = true;
}

void loop() {
  task30102(); //必須自己一個loop
}
int vs = 0;
void loop2 (void* pvParameters) {
  while(1){
    int val = digitalRead(btn);
    if(!val) vs++;
    if(vs>10){
      beepHz(1);
      startTime = millis();
      vs=0;
    }
    beepHz(0);
    
    if(start) showdata();
    if(dataUp){
      //dbdata();
      dataUp = false;
    }
  }
}