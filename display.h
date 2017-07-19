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

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "U8glib.h"

extern U8GLIB_SSD1306_128X64 u8g;

#define LARGE_FONT u8g_font_helvR12
#define SMALL_FONT u8g_font_profont10

#define LARGE_FONT_HEIGHT 14
#define SMALL_FONT_HEIGHT 8

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_LINE_HEIGHT SMALL_FONT_HEIGHT
#define OLED_LINES (OLED_HEIGHT / OLED_LINE_HEIGHT)
#define MAXIMUM_LINE_WIDTH 25

void initDisplay(void);

void drawDisplay(void (*drawFunc)(void));

void drawSplash(void);
void drawText(void);

void writeLine(int line, const char *s);
void writeLine(int line, String &s);

#define DRAW_TEXT() drawDisplay(drawText)
#define DRAW_SPLASH() drawDisplay(drawSplash)

#endif // __DISPLAY_H__

