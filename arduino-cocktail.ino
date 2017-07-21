/*
 * Arduino-Cocktail
 *
 * Main Logic
 * Copyright 2017 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */

#include "dispenser.h"
#include "display.h"
#include "ui.h"

#define BAUDRATE 115200
#define VERSION_ID "1.0"

void setup(void) {
    Serial.begin(BAUDRATE);
    Serial.println(); // skip garbage data after reset

    Serial.println(F("Initializing arduino-cocktail..."));
    initDispenser();

    Serial.println(F("Initializing OLED..."));
    initDisplay();

    DRAW_SPLASH();
    Serial.println(F("Showing splash screen..."));
    delay(2500);

    Serial.println(F("Preparing Menu interface..."));
    initUI();

    Serial.println(F("Finished. Entering main loop!"));
}

void loop(void) {
    dispenserThread();
    MenuEntry::menu();
}
