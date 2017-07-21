/*
 * Arduino-Cocktail
 *
 * OLED helpers
 * Copyright 2017 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */

#include <Arduino.h>
#include "display.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

char displayLines[OLED_LINES][MAXIMUM_LINE_WIDTH + 1];
bool startingDrawing = false;

void initDisplay(void) {
    for (int i = 0; i < OLED_LINES; i++) {
        for (int j = 0; j < MAXIMUM_LINE_WIDTH; j++) {
            displayLines[i][j] = ' ';
        }
        displayLines[i][MAXIMUM_LINE_WIDTH] = '\0';
    }
}

void drawDisplay(void (*drawFunc)(void)) {
    u8g.firstPage();
    startingDrawing = true;
    do {
        drawFunc();
        startingDrawing = false;
    } while(u8g.nextPage());
}

void drawSplash(void) {
    u8g.setFont(LARGE_FONT);
    u8g.drawStr(0, LARGE_FONT_HEIGHT, "Arduino");
    u8g.drawStr(0, 2 * LARGE_FONT_HEIGHT, "Cocktail");

    u8g.setFont(SMALL_FONT);
    u8g.drawStr(0, OLED_HEIGHT - (2 * SMALL_FONT_HEIGHT), "www.xythobuz.de");
    u8g.drawStr(0, OLED_HEIGHT - (1 * SMALL_FONT_HEIGHT), "License:");
    u8g.drawStr(0, OLED_HEIGHT - (0 * SMALL_FONT_HEIGHT), "Beer-Ware!");
}

void drawText(void) {
    u8g.setFont(SMALL_FONT);

    for (int i = 0; i < OLED_LINES; i++) {
        int l = u8g.drawStr(0, (i + 1) * OLED_LINE_HEIGHT, displayLines[i]);

        u8g.setColorIndex(0);
        u8g.drawBox(l, i * OLED_LINE_HEIGHT, OLED_WIDTH - l, OLED_LINE_HEIGHT);
        u8g.setColorIndex(1);

        if (startingDrawing) {
            Serial.print(F("OLED "));
            Serial.print(i + 1);
            Serial.print(F(" = "));
            Serial.println(displayLines[i]);
        }
    }
}

void writeLine(int line, const char *s) {
    if ((line >= 0) && (line < OLED_LINES)) {
        int j;
        for (j = 0; (j < MAXIMUM_LINE_WIDTH) && (s[j] != '\0'); j++) {
            displayLines[line][j] = s[j];
        }
        for (int i = j; i < MAXIMUM_LINE_WIDTH; i++) {
            displayLines[line][i] = ' ';
        }
    }
}

void writeLine(int line, const String &s) {
    writeLine(line, s.c_str());
}

void writeLine(int line, const __FlashStringHelper *s) {
    String st(s);
    writeLine(line, st.c_str());
}

