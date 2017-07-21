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

#include <Arduino.h>
#include "recipes.h"

// List of ingredients
// Valve Nr, Name, Alcohol in %, Caffeine in mg/100ml, Calories in kcal/100ml
Ingredient ingCoke      = { 0, "Coca-Cola",    0, 10,  37 };
Ingredient ingCokeLight = { 1, "Cola light",   0, 12,   1 };
Ingredient ingRum       = { 2, "Havana Club", 40,  0, 230 };
Ingredient ingMate      = { 3, "Club Mate",    0, 20,  20 };

Ingredient *ingredients[INGREDIENT_COUNT] = { &ingCoke, &ingCokeLight, &ingRum, &ingMate };

// List of recipes
// Name, size in ml, list of parts: ingredient pointer, percentage
Recipe recRumCola = { "Rum-Cola", 350, {
    { &ingCoke, 75 },
    { &ingRum, 25 },
    { NULL, 0 },
    { NULL, 0 }
}};
Recipe recRumColaLight = { "Rum-Cola-light", 350, {
    { &ingCokeLight, 75 },
    { &ingRum, 25 },
    { NULL, 0 },
    { NULL, 0 }
}};
Recipe recTschunk = { "Tschunk", 300, {
    { &ingMate, 79 },
    { &ingRum, 21 },
    { NULL, 0 },
    { NULL, 0 }
}};

Recipe *recipes[RECIPE_COUNT] = { &recRumCola, &recRumColaLight, &recTschunk };

