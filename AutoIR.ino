//--------------------------------------------------------------------------------
//! @file   AutoIR.ino
//! @brief  自動赤外線リモコン
//--------------------------------------------------------------------------------

#include "AutoIR.h"

namespace{
    // int PIN_ENCODER_A = 2;     // Encoder.hで予約済
    // int PIN_ENCODER_B = 3;     // Encoder.hで予約済
    int PIN_SW = 4;
    int PIN_LCD_BACKLIGHT = 5;
    int PIN_IR_SEND = 6;
}

AutoIR autoIR;

void setup() {
    AutoIR::InitArg initArg;
    {
      initArg.mPIN_SW = PIN_SW;
      initArg.mPIN_LCD_BACKLIGHT = PIN_LCD_BACKLIGHT;
      initArg.mPIN_IR_SEND = PIN_IR_SEND;
    }
    autoIR.setup(initArg);
}

void loop() {
    autoIR.loop();
}