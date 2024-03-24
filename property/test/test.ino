#include <Wire.h>
#include <MAX30100_PulseOximeter.h> //心率血氧脈搏感測器
#define HEART_RATE_BUFF_SIZE 10
#define PULSE_BUFF_SIZE      50
#define CHART_BUFF_SIZE      240
#define SENSOR_GET_TIMER_MS       800
#define PULSE_GET_TIMER_MS        20
#define CHART_CLEAR_TIMER_MS      2000
#define CHRG_BAT_ICON_TIMER_MS    500
#define DISCHRG_BAT_ICON_TIMER_MS 10000
#define SHOW_SENSOR_DATA_TIMER_MS 1000
#define FINGER_OUT_TIMEOUT_MS     7000

PulseOximeter pox; //感測器物件
uint8_t heart_rate_buff[HEART_RATE_BUFF_SIZE] = {0};
uint8_t spo2 = 0;
uint8_t heart_rate = 0;
bool detect_beat = false;
uint32_t chart_clear_timer = 0;

const char* data_format = "%5.1f";
const unsigned long lastingInterval = 35L * 1000L;
unsigned long lastConnectTime = 0L;

void setup(){
  Serial.begin(115200);
  MAX30100Init();
}

void MAX30100Init(void){ // Sensor 初始化
  if (!pox.begin()) {
    Serial.println("BAD");
    while (true) yield();
  } else {
    Serial.println("GOOD");
  }
  
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA); //LED Configuration
  pox.setOnBeatDetectedCallback(onBeatDetected); //Register a callback for the beat detection
}

void loop(){
  TaskMax30100();
}

void TaskMax30100(void){
  static uint32_t sensor_get_timer = 0;
  static uint32_t pulse_get_timer = 0;
  static uint8_t hr_buf_idx = 0;
  static float pulse_buff[PULSE_BUFF_SIZE] = {0.0f};
  static uint8_t pls_buf_idx = 0;
  
  pox.update();

  if (millis() > sensor_get_timer) {
    sensor_get_timer = millis() + SENSOR_GET_TIMER_MS;
    heart_rate_buff[hr_buf_idx++] = (uint8_t)pox.getHeartRate();
    spo2 = pox.getSpO2();
    
    if (hr_buf_idx >= HEART_RATE_BUFF_SIZE) hr_buf_idx = 0; //Check buff index

    uint16_t avg_hr = 0;
    for (uint8_t i = 0; i < HEART_RATE_BUFF_SIZE; i++) avg_hr += heart_rate_buff[i];
    heart_rate = (uint8_t)(avg_hr / HEART_RATE_BUFF_SIZE);
  }

  if (millis() > pulse_get_timer) {
    pulse_get_timer = millis() + PULSE_GET_TIMER_MS;
    pulse_buff[pls_buf_idx++] = pox.ftPlsVal; // Get sensor filtered Pulse Value
    
    if (pls_buf_idx >= PULSE_BUFF_SIZE) pls_buf_idx = 0; //Check buff index

    float min_avg = 0.0f, max_avg = 0.0f;
    uint8_t min_num = 0, max_num = 0;
    for (int i = 0; i < PULSE_BUFF_SIZE; i++){
      if (pulse_buff[i] < 0) { //min
          min_avg += pulse_buff[i];
          min_num++;
      }
      if (pulse_buff[i] > 0) { //max
          max_avg += pulse_buff[i];
          max_num++;
      }
    }

    min_avg /= min_num; //average
    max_avg /= max_num;

  }
}
void onBeatDetected(void){
  chart_clear_timer = millis() + CHART_CLEAR_TIMER_MS;
  detect_beat = true;
}
