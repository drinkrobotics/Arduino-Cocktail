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
#ifndef __MENU_H__
#define __MENU_H__

#define MENU_ENTRY_MAX_CHILDREN 10

class MenuEntry;

typedef int (*MenuDrawFunc)(MenuEntry *entry, void *userData);
typedef int (*MenuActFunc)(MenuEntry **entry, void *userData);

#define MENU_ERR_UNUSED -1
#define MENU_NO_ERR 0
#define MENU_SHOULD_DRAW 1
#define MENU_ERR_INVALID_ENTRY 2
#define MENU_ERR_OUT_OF_SPACE 3

int alwaysReturnActFunc(MenuEntry **entry, void *userData);

class MenuEntry {
  public:
    MenuEntry(const char *_name, MenuEntry *_parent = NULL,
            MenuDrawFunc _drawFunc = NULL, MenuActFunc _actFunc = NULL, void *_userData = NULL);

    void setDrawFunc(MenuDrawFunc func) { drawFunc = func; }
    void setActFunc(MenuActFunc func) { actFunc = func; }

    const char *getName(void) { return name; }
    MenuEntry *getParent(void) { return parent; }

    static int menu(MenuEntry **entry = &currentMainMenuItem);
    static void setMainMenuItem(MenuEntry *entry) { currentMainMenuItem = entry; }

  private:
    const char* name;
    MenuEntry *parent;
    MenuEntry *children[MENU_ENTRY_MAX_CHILDREN + 1];
    MenuDrawFunc drawFunc;
    MenuActFunc actFunc;
    void *userData;

    int childOffset = 0;
    int selection = 0;

    static MenuEntry *currentMainMenuItem;
    static bool firstDraw;

    int getChildCount(void);
    int drawChildrenList(void);
    int actChildrenList(MenuEntry **entry);
    int addChild(MenuEntry *child);

    int draw(void);
    int act(MenuEntry **entry);
};

#endif

