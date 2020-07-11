//--------------------------------------------------------------------------------
//! @file   AutoIR.ino
//! @brief  自動赤外線リモコン
//--------------------------------------------------------------------------------

// RTC
#include "RTClib.h"
// LCD
#include "LCD_ST7032.h"
// Encoder
#include "Encoder.h"

using RTC = RTC_DS3231;
using LCD = LCD_ST7032;

#define PIN_LCD_BACKLIGHT 3
#define PIN_ENCODER_A 4
#define PIN_ENCODER_B 5

RTC rtc;
LCD lcd;
Encoder encoder(PIN_ENCODER_A, PIN_ENCODER_B);
static int counter = 0;
DateTime alermTime;

//--------------------------------------------------------------------------------
//! @brief  初回起動時の設定
//--------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  initRTC(rtc);
  initLCD(lcd);
  initEncoder(encoder);

  setLCDBlightness(lcd,1.5f);
  rtc.setAlarm1(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(5), Ds3231Alarm1Mode::DS3231_A1_Second);
  
  Serial.println("初期化完了");
}

//--------------------------------------------------------------------------------
//! @brief  メインループ
//--------------------------------------------------------------------------------
void loop() {
  if(rtc.alarmFired(1)){
    dispLCD(lcd, "ALARM", 0);
  } else {
    char text[20] = "";
    getTimeStr(rtc, text);
    dispLCD(lcd, text, 0);
  }

  calcEncoderInput(encoder);
  char buf[20] = "hh:mm:ss";
  alermTime.toString(buf);
  dispLCD(lcd, buf, 1);

  // delay(1000);
}

//--------------------------------------------------------------------------------
//! @brief  RTCを初期化
//! @param  rtc RTCのインスタンス
//! @return 初期化に成功したか
//--------------------------------------------------------------------------------
bool initRTC(RTC& rtc) {
  if(!rtc.begin()){
    Serial.println("RTCの初期化に失敗");
    return false;
  }
  if(rtc.lostPower()){
    Serial.println("時刻を初期化します");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.clearAlarm(1);
  return true;
}

//--------------------------------------------------------------------------------
//! @brief  RTCの時刻を表示
//! @param  rtc RTCのインスタンス
//! @return
//--------------------------------------------------------------------------------
void dispTime(RTC& rtc){
  DateTime time = rtc.now();
  Serial.print(time.year(), DEC);
  Serial.print("/");
  Serial.print(time.month(), DEC);
  Serial.print("/");
  Serial.print(time.day(), DEC);
  Serial.print("/\n");

  Serial.print(time.hour(), DEC);
  Serial.print(":");
  Serial.print(time.minute(), DEC);
  Serial.print(":");
  Serial.print(time.second(), DEC);
  Serial.print("\n");
}

//--------------------------------------------------------------------------------
//! @brief   現在の時刻を文字列として取得
//! @param   rtc RTCのインスタンス
//! @param   outChar 出力先バッファのポインタ
//! @return  出力先バッファのポインタ
//--------------------------------------------------------------------------------
char* getTimeStr(RTC& rtc, char* outChar){
  DateTime time = rtc.now();
  char buf[] = "MM/DD hh:mm:ss";
  time.toString(buf);
  strcpy(outChar, buf);
  return outChar;
}

//--------------------------------------------------------------------------------
//! @brief  LCDを初期化
//! @param  lcd LCDのインスタンス
//! @return 初期化に成功したか
//--------------------------------------------------------------------------------
bool initLCD(LCD& lcd){
  lcd.begin();
  pinMode(PIN_LCD_BACKLIGHT, OUTPUT);
  return true;
}

//--------------------------------------------------------------------------------
//! @brief  LCDに文字を表示
//! @param  lcd LCDのインスタンス
//! @param  str 表示する文字列
//--------------------------------------------------------------------------------
void dispLCD(LCD& lcd, const char* str, int line){
  lcd.setCursor(line,0);
  lcd.print(str);
}

//--------------------------------------------------------------------------------
//! @brief  LCDの輝度を調整
//! @param  lcd LCDのインスタンス
//! @param  targetV セットする電圧（0～3.3V）
//--------------------------------------------------------------------------------
void setLCDBlightness(LCD& lcd, float targetV){
  if(targetV<0){
    targetV = 0.f;
  }
  if(targetV>3.3f){
    targetV = 3.3f;
  }
  int duty = 255 * (targetV/5.f);
  analogWrite(PIN_LCD_BACKLIGHT, duty);
}

//--------------------------------------------------------------------------------
//! @brief  ロータリーエンコーダを初期化
//! @param  encoder ロータリーエンコーダのインスタンス
//! @return 初期化に成功したか
//--------------------------------------------------------------------------------
bool initEncoder(Encoder& encoder){
  encoder.Timer_init();
  return true;
}

//--------------------------------------------------------------------------------
//! @brief   ロータリーエンコーダの入力を計算
//! @param   encoder ロータリーエンコーダのインスタンス
//--------------------------------------------------------------------------------
void calcEncoderInput(Encoder& encoder){
  // encoder
  TimeSpan deltaTime(0,0,1,0);
  if(encoder.rotate_flag==1){
    if(encoder.direct==1){
      alermTime = alermTime + deltaTime;
    } else{
      alermTime = alermTime -deltaTime;
    }
    encoder.rotate_flag = 0;
  }
}