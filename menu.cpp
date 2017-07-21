/*
 * Arduino-Cocktail
 *
 * OLED Menu
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
#include "debounce.h"
#include "menu.h"

extern Debouncer s1;
extern Debouncer s2;
extern Debouncer s3;

MenuEntry *MenuEntry::currentMainMenuItem = NULL;
bool MenuEntry::firstDraw = false;

int MenuEntry::menu(MenuEntry **entry) {
    int actRet = (*entry)->act(entry);

    int drawRet = MENU_ERR_UNUSED;
    if ((!firstDraw) || (actRet == MENU_SHOULD_DRAW)) {
        firstDraw = true;
        drawRet = (*entry)->draw();

        Serial.print(F("Menu: drawing ("));
        Serial.print(drawRet ? "0" : "1");
        Serial.print(F(" / "));
        Serial.print(actRet);
        Serial.println(F(")"));
    }

    return (drawRet != MENU_ERR_UNUSED) ? drawRet : actRet;
}

int alwaysReturnActFunc(MenuEntry **entry, void *userData) {
    int sw1 = s1.poll();
    int sw2 = s2.poll();
    int sw3 = s3.poll();

    if ((sw1 == -1) || (sw2 == -1) || (sw3 == -1)) {
        // always return to parent
        *entry = (*entry)->getParent();
        return MENU_SHOULD_DRAW;
    }

    return MENU_NO_ERR;
}

MenuEntry::MenuEntry(const char *_name, MenuEntry *_parent,
        MenuDrawFunc _drawFunc, MenuActFunc _actFunc, void *_userData)
        : name(_name), parent(_parent), drawFunc(_drawFunc), actFunc(_actFunc), userData(_userData) {
    children[0] = NULL;

    if (parent != NULL) {
        if (parent->addChild(this) != MENU_NO_ERR) {
            Serial.println(F("Menu-Error: couldn't add another child item!"));
            parent = NULL;
        }
    }

    if (currentMainMenuItem == NULL) {
        // automatically set first item to be main menu root
        currentMainMenuItem = this;
    }
}

int MenuEntry::getChildCount(void) {
    int childCount = 0;
    while (children[childCount] != NULL) childCount++;
    return childCount;
}

int MenuEntry::addChild(MenuEntry *child) {
    int childCount = getChildCount();
    if (childCount < MENU_ENTRY_MAX_CHILDREN) {
        children[childCount] = child;
        children[childCount + 1] = NULL;
        return MENU_NO_ERR;
    } else {
        return MENU_ERR_OUT_OF_SPACE;
    }
}

int MenuEntry::drawChildrenList(void) {
    int realChildCount = getChildCount();

    // Show "Back..." at end of menu when parent menu exists
    int childCount = realChildCount;
    if (parent != NULL) childCount++;

    int i;
    for (i = childOffset; (i < childCount) && (i < (childOffset + 7)); i++) {
        String line;
        if (selection == i) {
            line += "= ";
        } else {
            line += "  ";
        }

        if (i < realChildCount) {
            line += children[i]->name;
        } else {
            line += "Back...";
        }

        writeLine(1 + i - childOffset, line.c_str());
    }

    // Clear screen below menu entries
    i -= childOffset;
    if (i < 7) {
        for (int j = i + 1; j < OLED_LINES; j++) {
            writeLine(j, "");
        }
    }

    return MENU_NO_ERR;
}

int MenuEntry::draw(void) {
    int ret = MENU_NO_ERR;
    if (drawFunc != NULL) {
        ret = drawFunc(this, userData);
    } else if (children != NULL) {
        writeLine(0, name);
        ret = drawChildrenList();
        DRAW_TEXT();
    } else {
        ret = MENU_ERR_INVALID_ENTRY;
    }

    return ret;
}

int MenuEntry::actChildrenList(MenuEntry **entry) {
    bool redraw = false;
    
    int sw1 = s1.poll();
    int sw2 = s2.poll();
    int sw3 = s3.poll();

    int realChildCount = getChildCount();
    
    // Show "Back..." at end of menu when parent menu exists
    int childCount = realChildCount;

    if (sw1 == -1) {
        // Left button
        selection--;
        redraw = true;
        Serial.println(F("Menu: moving left"));
    }
    
    if (sw3 == -1) {
        // Right button
        selection++;
        redraw = true;
        Serial.println(F("Menu: moving right"));
    }

    // Fix over- or under-flown selection index
    if (parent != NULL) childCount++;
    if (selection < 0) {
        selection = (childCount > 0) ? (childCount - 1) : 0;
    }
    if (selection >= childCount) {
        selection = 0;
    }

    while (selection < childOffset) {
        childOffset--;
    }
    while (selection >= (childOffset + 7)) {
        childOffset++;
    }

    if (sw2 == -1) {
        // Center button
        if (selection >= realChildCount) {
            // Go up to parent menu...
            *entry = parent;
            Serial.println(F("Menu: entering parent menu"));
        } else {
            // Enter sub menu...
            *entry = children[selection];
            Serial.println(F("Menu: entering sub menu"));
        }
        
        redraw = true;
        selection = 0;
    }
    
    return redraw ? MENU_SHOULD_DRAW : MENU_NO_ERR;
}

int MenuEntry::act(MenuEntry **entry) {
    if (actFunc != NULL) {
        return actFunc(entry, userData);
    } else if (children != NULL) {
        return actChildrenList(entry);
    } else {
        return MENU_ERR_INVALID_ENTRY;
    }
}

