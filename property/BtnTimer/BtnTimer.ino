#define btn 0

int n = 1; //test
unsigned long runTime, startTime;
int Tsec, Tmin, Thr;
int val;
void setup() {
  Serial.begin(115200);
  pinMode(btn, INPUT);
}

void loop() {
  val=digitalRead(btn);
  if(!val) startTime = millis();
  Tsec = Tmin = Thr = 0.0; 
  runTime = millis() - startTime;      //讀取 Arduino 板執行時間
  Tsec = n * runTime / 1000 % 60;
  Tmin = n * runTime / 1000 / 60 % 60;
  Thr = n * runTime / 1000 / 60 / 60 % 60;
  Serial.print("time: ");  //回傳時間
  if(Thr<10) {
    Serial.print("0"); 
    Serial.print(Thr);
  }
  else Serial.print(Thr); 
  Serial.print(":");
  if(Tmin<10) {
    Serial.print("0"); 
    Serial.print(Tmin); 
  }
  else Serial.print(Tmin);    
  Serial.print(":"); 
  if(Tsec<10) {
    Serial.print("0"); 
    Serial.print(Tsec);
  }
  else Serial.print(Tsec);
  Serial.println();  
}