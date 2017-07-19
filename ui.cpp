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

#include <Arduino.h>
#include "recipes.h"
#include "dispenser.h"
#include "ui.h"

Debouncer s1(S1_INPUT);
Debouncer s2(S2_INPUT);
Debouncer s3(S3_INPUT);

MenuEntry mainMenuEntry("Main Menu");
MenuEntry autoMixMenuEntry("Auto Mixing", &mainMenuEntry);
MenuEntry manualMixMenuEntry("Manual Mixing", &mainMenuEntry);
MenuEntry ingredientsMenuEntry("Ingredient List", &mainMenuEntry);

MenuEntry manualMix1MenuEntry("Valve 1 & 2", &manualMixMenuEntry, manualDrawFunc, manualActFunc, (void *)(0x01 | 0x02));
MenuEntry manualMix2MenuEntry("Valve 3 & 4", &manualMixMenuEntry, manualDrawFunc, manualActFunc, (void *)(0x04 | 0x08));

MenuEntry *ingredientMenuEntries[INGREDIENT_COUNT];
MenuEntry *recipeAutoMenuEntries[RECIPE_COUNT];

static bool dispensed = false;
static bool showStats = false;

void initUI(void) {
    pinMode(S1_INPUT, INPUT);
    pinMode(S2_INPUT, INPUT);
    pinMode(S3_INPUT, INPUT);

    // Pull ups for push buttons enabled
    digitalWrite(S1_INPUT, HIGH);
    digitalWrite(S2_INPUT, HIGH);
    digitalWrite(S3_INPUT, HIGH);

    // ignore initial state change
    s1.poll();
    s2.poll();
    s3.poll();
    delay(DEBOUNCE_DELAY + 2);
    s1.poll();
    s2.poll();
    s3.poll();

    for (int i = 0; i < INGREDIENT_COUNT; i++) {
        Serial.println(F("Creating Ingredient menu entry..."));
        
        ingredientMenuEntries[i] = new MenuEntry(ingredients[i]->name,
                &ingredientsMenuEntry, ingredientDrawFunc, alwaysReturnActFunc, ingredients[i]);
    }

    for (int i = 0; i < RECIPE_COUNT; i++) {
        Serial.println(F("Creating recipe menu entry..."));
        
        recipeAutoMenuEntries[i] = new MenuEntry(recipes[i]->name,
                &autoMixMenuEntry, dispenseDrawFunc, dispenseActFunc, recipes[i]);
    }
}

int manualDrawFunc(MenuEntry *entry, void *userData) {
    int valvesel = (int)userData;

    String name("Valves:");
    int l = 1;
    for (int i = 0; i < 4; i++) {
        if (valvesel & (1 << i)) {
            name += " ";
            name += String(i + 1);
            writeLine(l++, ingredients[i]->name);
        }
    }

    writeLine(0, name);
    l++;
    writeLine(l++, "L & R to dispense");
    writeLine(l++, "Center to exit!");

    DRAW_TEXT();
    return MENU_NO_ERR;
}

int manualActFunc(MenuEntry **entry, void *userData) {
    int valvesel = (int)userData;

    int sw1 = s1.poll();
    int sw2 = s2.poll();
    int sw3 = s3.poll();

    if (sw2 == -1) {
        for (int i = 0; i < 4; i++) {
            digitalWrite(valves[i], HIGH);
        }
        *entry = (*entry)->getParent();
        return MENU_SHOULD_DRAW;
    }

    int sw = 0;
    for (int i = 0; i < 4; i++) {
        if (valvesel & (1 << i)) {
            if (sw == 0) {
                sw++;
                if (sw1 == -1) {
                    digitalWrite(valves[i], LOW);
                } else if (sw1 == 1) {
                    digitalWrite(valves[i], HIGH);
                }
            } else {
                if (sw3 == -1) {
                    digitalWrite(valves[i], LOW);
                } else if (sw3 == 1) {
                    digitalWrite(valves[i], HIGH);
                }
            }
        }
    }

    return MENU_NO_ERR;
}

int ingredientDrawFunc(MenuEntry *entry, void *userData) {
    Ingredient *ing = (Ingredient *)userData;

    writeLine(0, entry->getName());
    writeLine(1, String("Valve No: ") + String(ing->valve));
    writeLine(2, "");
    writeLine(3, String(" Alcohol: ") + String(ing->alcohol) + "%");
    writeLine(4, "");
    writeLine(5, " / 100ml:");
    writeLine(6, String("Caffeine: ") + String(ing->caffeine) + "mg");
    writeLine(7, String("Calories: ") + String(ing->calories) + "kcal");

    DRAW_TEXT();

    return MENU_NO_ERR;
}

#define FLOW_RATE (1000 / 50) // 1000ms / 50ml

static int lastDispensionTimes[4] = { 0, 0, 0, 0 };

int dispenseActFunc(MenuEntry **entry, void *userData) {
    static unsigned long progressTime = 0;

    Recipe *recipe = (Recipe *)userData;

    int sw1 = s1.poll();
    int sw2 = s2.poll();
    int sw3 = s3.poll();

    bool dispensing = isDispensing();

    // (Third) continue when dispensing has finished
    if (!dispensing && dispensed && !showStats) {
        // redraw with stats when finished
        dispensed = false;
        showStats = true;
        return MENU_SHOULD_DRAW;
    }

    // (Second) show the progress meter while waiting
    // for the dispensing action to finish
    if (dispensing && dispensed && !showStats) {
        // repeatedly update progress indicator...
        unsigned long currentTime = millis();
        if ((currentTime - progressTime) > PROGRESS_INTERVAL) {
            progressTime = currentTime;
            return MENU_SHOULD_DRAW;
        } else {
            return MENU_NO_ERR;
        }
    }

    // Only continue when a button has been pressed
    if ((sw1 != -1) && (sw2 != -1) && (sw3 != -1)) {
        return MENU_NO_ERR;
    }

    // (First) show the drink summary, wait for GO signal
    if (!dispensing && !dispensed && !showStats) {
        if (sw2 == -1) {
            // start dispenser, calculate timings
            lastDispensionTimes[0] = 0;
            lastDispensionTimes[1] = 0;
            lastDispensionTimes[2] = 0;
            lastDispensionTimes[3] = 0;
            for (int i = 0; i < 4; i++) {
                if (recipe->parts[i].ingredient != NULL) {
                    int targetSize = recipe->parts[i].percentage * recipe->size / 100;
                    int targetTime = targetSize * FLOW_RATE;
                    lastDispensionTimes[recipe->parts[i].ingredient->valve] = targetTime;
                }
            }
            startDispensing(lastDispensionTimes[0], lastDispensionTimes[1], lastDispensionTimes[2], lastDispensionTimes[3]);
            dispensed = true;
            progressTime = millis();
        } else {
            // allow user to select drink size
            if (sw1 == -1) {
                if (recipe->size > DRINK_SIZE_MINIMUM) {
                    recipe->size -= SIZE_MODIFIER_STEP;
                }
            }

            if (sw3 == -1) {
                if (recipe->size < DRINK_SIZE_MAXIMUM) {
                    recipe->size += SIZE_MODIFIER_STEP;
                }
            }
        }

        return MENU_SHOULD_DRAW;
    }

    // (Fourth) show the stats about the drink
    // wait for button to return to main menu
    if (!dispensing && !dispensed && showStats) {
        // return to main menu when finished
        *entry = (*entry)->getParent()->getParent();
        showStats = false;
        return MENU_SHOULD_DRAW;
    }

    return MENU_NO_ERR;
}

static int dispenseDrawDrinkInfo(Recipe *recipe) {
    uint32_t sizePx = ((uint32_t)recipe->size /* - DRINK_SIZE_MINIMUM */) * OLED_WIDTH / (DRINK_SIZE_MAXIMUM /* - DRINK_SIZE_MINIMUM */);

    String sizeInfo("Size: ");
    sizeInfo += recipe->size;
    sizeInfo += "ml";

    u8g.firstPage();
    do {
        // draw recipe name heading
        u8g.setFont(LARGE_FONT);
        u8g.drawStr(0, LARGE_FONT_HEIGHT, recipe->name);

        // draw list of ingredients
        u8g.setFont(SMALL_FONT);
        int yOff = LARGE_FONT_HEIGHT + SMALL_FONT_HEIGHT; // (20px + 8px) = 28px
        for (int i = 0; (i < MAX_RECIPE_PARTS) && (recipe->parts[i].ingredient != NULL); i++) {
            String name(recipe->parts[i].ingredient->name);
            name += ": " + String(recipe->parts[i].percentage) + "%";
            u8g.drawStr(0, yOff, name.c_str());

            yOff += SMALL_FONT_HEIGHT;
        }

        yOff -= SMALL_FONT_HEIGHT - 2; // leave 2px distance between list and bar

        // show size indicator "progress bar"
        u8g.drawBox(0, yOff, sizePx, OLED_HEIGHT - yOff);

        // clear end of progress bar
        u8g.setColorIndex(0);
        u8g.drawBox(sizePx, yOff, OLED_WIDTH - sizePx, OLED_HEIGHT - yOff);

        int fontHeight = SMALL_FONT_HEIGHT;
        if ((OLED_HEIGHT - yOff) > (LARGE_FONT_HEIGHT + (2 * SIZE_SELECTOR_Y_DIST))) {
            u8g.setFont(LARGE_FONT);
            fontHeight = LARGE_FONT_HEIGHT;
        }
        int w = u8g.getStrWidth(sizeInfo.c_str());

        // blank spot for text overlay
        yOff = yOff + ((OLED_HEIGHT - yOff - fontHeight) / 2);
        u8g.drawBox((OLED_WIDTH - w) / 2 - SIZE_SELECTOR_X_DIST, yOff - SIZE_SELECTOR_Y_DIST,
                w + (2 * SIZE_SELECTOR_X_DIST), fontHeight + (2 * SIZE_SELECTOR_Y_DIST));

        // text overlay with current ml selection
        u8g.setColorIndex(1);
        u8g.drawStr((OLED_WIDTH - w) / 2, yOff + fontHeight, sizeInfo.c_str());
    } while(u8g.nextPage());

    return MENU_NO_ERR;
}

static int dispenseDrawProgress(Recipe *recipe) {
    int progress = dispensionProgress();
    String prog(progress);
    prog += "%";

    int heightPx = ((uint32_t)progress) * GLASS_ANIM_HEIGHT / 100;

    u8g.firstPage();
    do {
        u8g.setFont(LARGE_FONT);
        u8g.drawStr(50, (1 * LARGE_FONT_HEIGHT), "Dispensing");
        u8g.drawStr(50, (2 * LARGE_FONT_HEIGHT) + (1 * GLASS_TEXT_OFF), "Wait!");
        u8g.drawStr(50, (3 * LARGE_FONT_HEIGHT) + (2 * GLASS_TEXT_OFF), prog.c_str());

        u8g.drawHLine(GLASS_ANIM_OFF_X, OLED_HEIGHT - GLASS_ANIM_OFF_Y, GLASS_ANIM_WIDTH);
        u8g.drawVLine(GLASS_ANIM_OFF_X, GLASS_ANIM_OFF_Y, GLASS_ANIM_HEIGHT);
        u8g.drawVLine(GLASS_ANIM_OFF_X + GLASS_ANIM_WIDTH, GLASS_ANIM_OFF_Y, GLASS_ANIM_HEIGHT);

        if (heightPx > 0) {
            u8g.drawBox(GLASS_ANIM_OFF_X, GLASS_ANIM_OFF_Y + GLASS_ANIM_HEIGHT - heightPx, GLASS_ANIM_WIDTH, heightPx);
        }
    } while(u8g.nextPage());

    return MENU_NO_ERR;
}

static int dispenseShowStats(Recipe *recipe) {
    // after turning off the magnetic valves and relais,
    // there seems to be a voltage spike that causes the display
    // to not update properly...
    delay(250); // just wait a bit for this to pass

    String alcohol("Alcohol: ");
    String caffeine("Caffeine: ");
    String calories("Calories: ");

    uint32_t alcoholPercent = 0;
    uint32_t caffeineCont = 0;
    uint32_t caloriesCont = 0;
    for (int i = 0; i < 4; i++) {
        if (recipe->parts[i].ingredient != NULL) {
            alcoholPercent += (uint32_t)recipe->parts[i].ingredient->alcohol * (uint32_t)recipe->parts[i].percentage / 100ul;
            caffeineCont += (uint32_t)recipe->size * (uint32_t)recipe->parts[i].percentage / 100ul * (uint32_t)recipe->parts[i].ingredient->caffeine / 100ul;
            caloriesCont += (uint32_t)recipe->size * (uint32_t)recipe->parts[i].percentage / 100ul * (uint32_t)recipe->parts[i].ingredient->calories / 100ul;
        }
    }

    alcohol += String(alcoholPercent);
    alcohol += "% = ";
    alcohol += String(alcoholPercent * (uint32_t)recipe->size / 100ul);
    alcohol += "ml";

    caffeine += String(caffeineCont);
    caffeine += "mg";

    calories += String(caloriesCont);
    calories += "kcal";

    u8g.firstPage();
    do {
        u8g.setFont(LARGE_FONT);
        u8g.drawStr(0, LARGE_FONT_HEIGHT, "Enjoy your drink!");

        u8g.setFont(SMALL_FONT);
        u8g.drawStr(0, LARGE_FONT_HEIGHT + (2 * SMALL_FONT_HEIGHT), "Statistics:");
        u8g.drawStr(0, LARGE_FONT_HEIGHT + (3 * SMALL_FONT_HEIGHT), alcohol.c_str());
        u8g.drawStr(0, LARGE_FONT_HEIGHT + (4 * SMALL_FONT_HEIGHT), caffeine.c_str());
        u8g.drawStr(0, LARGE_FONT_HEIGHT + (5 * SMALL_FONT_HEIGHT), calories.c_str());
    } while(u8g.nextPage());

    return MENU_NO_ERR;
}

int dispenseDrawFunc(MenuEntry *entry, void *userData) {
    bool dispensing = isDispensing();

    if (!dispensing && !dispensed && !showStats) {
        // show drink info and size selection
        return dispenseDrawDrinkInfo((Recipe *)userData);
    }

    if (dispensing && dispensed && !showStats) {
        // show progress if dispenser is running
        return dispenseDrawProgress((Recipe *)userData);
    }

    if (!dispensing && !dispensed && showStats) {
        // show drink stats when finished
        return dispenseShowStats((Recipe *)userData);
    }

    return MENU_NO_ERR;
}
