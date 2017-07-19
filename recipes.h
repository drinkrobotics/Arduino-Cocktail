/*
 * Arduino-Cocktail
 *
 * Recipes
 * Copyright 2017 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */

#ifndef __RECIPES_H__
#define __RECIPES_H__

#define MAX_RECIPE_PARTS 4

struct Ingredient {
    uint8_t valve;
    const char *name;
    uint16_t alcohol; // alcohol content by volume, 0%-100%
    uint16_t caffeine; // mg caffeine per 100ml
    uint16_t calories; // kcal per 100ml
};

struct RecipePart {
    const Ingredient *ingredient;
    uint16_t percentage;
};

struct Recipe {
    const char *name;
    uint16_t size; // in ml
    RecipePart parts[MAX_RECIPE_PARTS];
};

#define INGREDIENT_COUNT 4
extern Ingredient *ingredients[INGREDIENT_COUNT];

#define RECIPE_COUNT 3
extern Recipe *recipes[RECIPE_COUNT];

#endif // __RECIPES_H__

