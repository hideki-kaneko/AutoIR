//--------------------------------------------------------------------------------
// @file   AutoIR.cpp
//--------------------------------------------------------------------------------

#include "AutoIR.h"
#include <EEPROM.h>

namespace{
    int cEEPROMAddress_AlarmTime = 0;
}

//--------------------------------------------------------------------------------
bool AutoIR::setup(const InitArg& arg){
    mInitArg = arg;
    Serial.begin(9600);
    initRTC_();
    initLCD_();
    initEncoder_();
    initSW_();

    setLCDBlightness_(1.5f);
    loadAlarmTime_();
    mState = isSwitchON_()? State::Active: State::Edit;

    Serial.println("初期化完了");
}
//--------------------------------------------------------------------------------
void AutoIR::loop(){
    calcEncoderInput_();
    updateLCD_();
    updateState_();
}
//--------------------------------------------------------------------------------
bool AutoIR::initRTC_(){
    if(!mRTC.begin()){
        Serial.println("RTCの初期化に失敗");
        return false;
    }
    if(mRTC.lostPower()){
        Serial.println("時刻を初期化します");
        mRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    mRTC.clearAlarm(1);
    return true;
}
//--------------------------------------------------------------------------------
bool AutoIR::initLCD_(){
    mLCD.begin();
    pinMode(mInitArg.mPIN_LCD_BACKLIGHT, OUTPUT);
    return true;
}
//--------------------------------------------------------------------------------
void AutoIR::updateLCD_(){
    
    // ステータスを表示
    if(mState == State::Active){
        mLCD.setCursor(1,0);
        mLCD.print("ALM ON");
    } else if(mState == State::Edit){
        mLCD.setCursor(1,0);
        mLCD.print("EDIT");
        mLCD.setCursor(1,7);
        mLCD.print(">");
    }

    // 日時を表示
    DateTime now = mRTC.now();
    char strCurDate[20] = "MM/DD";
    now.toString(strCurDate);
    mLCD.setCursor(0,0);
    mLCD.print(strCurDate);

    // 時刻を取得
    char strCurTime[20] = "hh:mm:ss";
    now.toString(strCurTime);
    mLCD.setCursor(0,8);
    mLCD.print(strCurTime);

    // アラームのトリガーを表示
    if(mRTC.alarmFired(1)){
        mLCD.setCursor(1,0);
        mLCD.print("ALARM");
        mRTC.clearAlarm(1);
        delay(1000);
        mLCD.clear();
    }

    // アラームの時刻を表示
    char strAlarmTime[20] = "hh:mm:ss";
    mAlarmTime.toString(strAlarmTime);
    mLCD.setCursor(1,8);
    mLCD.print(strAlarmTime);
}
//--------------------------------------------------------------------------------
void AutoIR::setLCDBlightness_(float targetV){
    if(targetV<0){
        targetV = 0.f;
    }
    if(targetV>3.3f){
        targetV = 3.3f;
    }
    int duty = 255 * (targetV/5.f);
    analogWrite(mInitArg.mPIN_LCD_BACKLIGHT, duty);
}
//--------------------------------------------------------------------------------
bool AutoIR::initEncoder_(){
  encoder.Timer_init();
  return true;
}
//--------------------------------------------------------------------------------
void AutoIR::calcEncoderInput_(){
    if(mState != State::Edit){
        return;
    }

    TimeSpan deltaTime(0,0,1,0);
    DateTime now = mRTC.now();
    if(encoder.rotate_flag==1){
        if(encoder.direct==1){
            mAlarmTime = mAlarmTime + deltaTime;
        } else{
            DateTime minTime(now.year(), now.month(), now.day(), 0, 0, 0);
            if( mAlarmTime > minTime ){
                mAlarmTime = mAlarmTime - deltaTime;
            }
        }
        encoder.rotate_flag = 0;
    }
}
//--------------------------------------------------------------------------------
bool AutoIR::initSW_(){
    pinMode(mInitArg.mPIN_SW, INPUT_PULLUP);
    return true;
}
//--------------------------------------------------------------------------------
bool AutoIR::isSwitchON_() const {
    return digitalRead(mInitArg.mPIN_SW);
}
//--------------------------------------------------------------------------------
void AutoIR::updateState_(){
    const State newState = isSwitchON_()? State::Active: State::Edit;
    if(newState != mState){
        mLCD.clear();
        if( newState == State::Active ){
            saveAlarmTime_();
        }
    }
    mState = newState;
}
//--------------------------------------------------------------------------------
void AutoIR::saveAlarmTime_(){
    EEPROM.put(cEEPROMAddress_AlarmTime, mAlarmTime);
    Serial.print("アラームの時刻をセーブ(");
    Serial.print(sizeof(mAlarmTime));
    Serial.println("bytes)");
    mRTC.setAlarm1(mAlarmTime, Ds3231Alarm1Mode::DS3231_A1_Hour & Ds3231Alarm1Mode::DS3231_A1_Minute & Ds3231Alarm1Mode::DS3231_A1_Second);
}
//--------------------------------------------------------------------------------
void AutoIR::loadAlarmTime_(){
    EEPROM.get(cEEPROMAddress_AlarmTime, mAlarmTime);
    Serial.println("アラームの時刻をロード");
    if(mAlarmTime.isValid()){
        mRTC.setAlarm1(mAlarmTime, Ds3231Alarm1Mode::DS3231_A1_Hour & Ds3231Alarm1Mode::DS3231_A1_Minute & Ds3231Alarm1Mode::DS3231_A1_Second);
    } else {
        Serial.println("データの読み込みに失敗しました");
        mAlarmTime = mRTC.now();
        mRTC.clearAlarm(1);
    }
}
//--------------------------------------------------------------------------------