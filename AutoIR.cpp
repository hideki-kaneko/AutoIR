//--------------------------------------------------------------------------------
// @file   AutoIR.cpp
//--------------------------------------------------------------------------------

#include "AutoIR.h"

//--------------------------------------------------------------------------------
bool AutoIR::setup(const InitArg& arg){
    mInitArg = arg;
    Serial.begin(9600);
    initRTC_();
    initLCD_();
    initEncoder_();
    initSW_();

    setLCDBlightness_(1.5f);
    mRTC.setAlarm1(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(5), Ds3231Alarm1Mode::DS3231_A1_Second);
    mAlarmTime = mRTC.now();

    Serial.println("初期化完了");
}
//--------------------------------------------------------------------------------
void AutoIR::loop(){
    calcEncoderInput_();
    updateLCD_();
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
    String status = "ALM:";
    status += (isSwitchON_() ? "ON ": "OFF");
    mLCD.setCursor(0,0);
    mLCD.print(status);

    // 時刻を取得
    char strCurTime[20] = "hh:mm:ss";
    DateTime curTime = mRTC.now();
    curTime.toString(strCurTime);
    mLCD.setCursor(0,8);
    mLCD.print(strCurTime);

    // アラームのトリガーを表示
    if(mRTC.alarmFired(1)){
        mLCD.setCursor(1,0);
        mLCD.print("ALARM");
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
    Serial.println(digitalRead(mInitArg.mPIN_SW));
    return digitalRead(mInitArg.mPIN_SW);
}
//--------------------------------------------------------------------------------