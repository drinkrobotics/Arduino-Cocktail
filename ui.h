/*
 * Arduino-Cocktail
 *
 * User Interface
 * Copyright 2017 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */

#ifndef __UI_H__
#define __UI_H__

#include "debounce.h"
#include "menu.h"
#include "display.h"

// Pin assignments for input switches
#define S1_INPUT 8
#define S2_INPUT 10
#define S3_INPUT 9

#define PROGRESS_INTERVAL 200 // in ms
#define SIZE_MODIFIER_STEP 50 // in ml
#define DRINK_SIZE_MINIMUM 0 // in ml
#define DRINK_SIZE_MAXIMUM 500 // in ml

// Pixel sizes for graphical elements
#define SIZE_SELECTOR_X_DIST 5
#define SIZE_SELECTOR_Y_DIST 2
#define GLASS_ANIM_OFF_X 5
#define GLASS_ANIM_OFF_Y 5
#define GLASS_TEXT_OFF 5
#define GLASS_ANIM_WIDTH 35
#define GLASS_ANIM_HEIGHT (OLED_HEIGHT - (2 * GLASS_ANIM_OFF_Y))

extern Debouncer s1;
extern Debouncer s2;
extern Debouncer s3;

void initUI(void);

int manualActFunc(MenuEntry **entry, void *userData);
int manualDrawFunc(MenuEntry *entry, void *userData);

int ingredientDrawFunc(MenuEntry *entry, void *userData);

int dispenseActFunc(MenuEntry **entry, void *userData);
int dispenseDrawFunc(MenuEntry *entry, void *userData);

#endif

