#define btn 0
#define beep 15
int val;
int mode = 0;
float modeArr[4] = {10000, 353.36, 333.3, 316.46};
void setup()  
{        
  Serial.begin(115200);
  pinMode(beep, OUTPUT);
  pinMode(btn, INPUT);
}

void loop()                     
{
  val=digitalRead(btn);
  if(!val && ++mode == 4) mode = 0;  
  if(mode != 0) beepHz(mode);
  else delay(1000);  
}

void beepHz(int hz)
{
  digitalWrite(beep, HIGH);
  delay(10); 
  digitalWrite(beep, LOW);
  delay(modeArr[hz]-10);
}