//--------------------------------------------------------------------------------
//! @file   AutoIR.ino
//! @brief  自動赤外線リモコン
//--------------------------------------------------------------------------------

#include "AutoIR.h"

namespace{
    int PIN_SW = 2;
    int PIN_LCD_BACKLIGHT = 3;
    int PIN_ENCODER_A = 4;
    int PIN_ENCODER_B = 5;
}

AutoIR autoIR;

void setup() {
    AutoIR::InitArg initArg;
    {
      initArg.mPIN_SW = PIN_SW;
      initArg.mPIN_LCD_BACKLIGHT = PIN_LCD_BACKLIGHT;
      initArg.mPIN_ENCODER_A = PIN_ENCODER_A;
      initArg.mPIN_ENCODER_B = PIN_ENCODER_B;
    }
    autoIR.setup(initArg);
}

void loop() {
    autoIR.loop();
}