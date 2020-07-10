//--------------------------------------------------------------------------------
//! @file   AutoIR.ino
//! @brief	自動赤外線リモコン
//--------------------------------------------------------------------------------

// RTC
#include "RTClib.h"

RTC_DS3231 rtc;

//--------------------------------------------------------------------------------
//! @brief		初回起動時の設定
//--------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  initRTC(rtc);
  Serial.println("初期化完了");
}

//--------------------------------------------------------------------------------
//! @brief		メインループ
//--------------------------------------------------------------------------------
void loop() {
  dispTime(rtc);
  delay(1000);
}

//--------------------------------------------------------------------------------
//! @brief		RTCを初期化
//! @param    rtc RTCのインスタンス
//! @return   初期化に成功したか
//--------------------------------------------------------------------------------
bool initRTC(RTC_DS3231& rtc) {
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
//! @brief		RTCの時刻を表示
//! @param		rtc RTCのインスタンス
//! @return		
//--------------------------------------------------------------------------------
void dispTime(RTC_DS3231& rtc){
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
