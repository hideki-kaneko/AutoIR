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

    Serial.println("初期化完了");
}
//--------------------------------------------------------------------------------
void AutoIR::loop(){
    if(mRTC.alarmFired(1)){
        dispLCD_("ALARM", 0);
    } else {
        char text[20] = "";
        getTimeStr_(text);
        dispLCD_(text, 0);
    }

    calcEncoderInput_();
    char buf[20] = "hh:mm:ss";
    mAlarmTime.toString(buf);
    dispLCD_(buf, 1);
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
char* AutoIR::getTimeStr_(char* outChar){
    DateTime time = mRTC.now();
    char buf[] = "MM/DD hh:mm:ss";
    time.toString(buf);
    strcpy(outChar, buf);
    return outChar;
}
//--------------------------------------------------------------------------------
bool AutoIR::initLCD_(){
    mLCD.begin();
    pinMode(mInitArg.mPIN_LCD_BACKLIGHT, OUTPUT);
    return true;
}
//--------------------------------------------------------------------------------
void AutoIR::dispLCD_(const char* str, int line){
    mLCD.setCursor(line,0);
    mLCD.print(str);
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
  mEncoder.Timer_init(mInitArg.mPIN_ENCODER_A, mInitArg.mPIN_ENCODER_B);
  return true;
}
//--------------------------------------------------------------------------------
void AutoIR::calcEncoderInput_(){
    TimeSpan deltaTime(0,0,1,0);
    if(mEncoder.rotate_flag==1){
        if(mEncoder.direct==1){
            mAlarmTime = mAlarmTime + deltaTime;
        } else{
            mAlarmTime = mAlarmTime - deltaTime;
        }
        mEncoder.rotate_flag = 0;
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