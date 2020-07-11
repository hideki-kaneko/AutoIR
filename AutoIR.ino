//--------------------------------------------------------------------------------
//! @file   AutoIR.ino
//! @brief  自動赤外線リモコン
//--------------------------------------------------------------------------------

// RTC
#include "RTClib.h"
// LCD
#include "LCD_ST7032.h"

using RTC = RTC_DS3231;
using LCD = LCD_ST7032;

#define PIN_LCD_BACKLIGHT 3

RTC rtc;
LCD lcd;

//--------------------------------------------------------------------------------
//! @brief  初回起動時の設定
//--------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  initRTC(rtc);
  initLCD(lcd);
  setLCDBlightness(lcd,1.5f);
  Serial.println("初期化完了");
}

//--------------------------------------------------------------------------------
//! @brief  メインループ
//--------------------------------------------------------------------------------
void loop() {
  dispTime(rtc);
  const char text[] = "hello";
  dispLCD(lcd, text);
  delay(1000);
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
void dispLCD(LCD& lcd, const char* str){
  lcd.setCursor(0,0);
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