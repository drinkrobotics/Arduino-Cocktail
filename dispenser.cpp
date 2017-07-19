/*
 * Arduino-Cocktail
 *
 * Dispenser
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */

#include <Arduino.h>
#include "dispenser.h"

static bool dispensing = false;
static unsigned long startTime = 0;

static int timings[VALVE_COUNT] = { 0, 0, 0, 0 };
int valves[VALVE_COUNT] = { V1_OUTPUT, V2_OUTPUT, V3_OUTPUT, V4_OUTPUT };

void initDispenser(void) {
    for (int i = 0; i < VALVE_COUNT; i++) {
        // Immediately disable valves
        pinMode(valves[i], OUTPUT);
        digitalWrite(valves[i], HIGH);
    }
}

void startDispensing(int t1, int t2, int t3, int t4) {
    startTime = millis();

    timings[0] = t1;
    timings[1] = t2;
    timings[2] = t3;
    timings[3] = t4;

    Serial.print(F("Starting dispenser: "));
    Serial.print(t1);
    Serial.print(F(", "));
    Serial.print(t2);
    Serial.print(F(", "));
    Serial.print(t3);
    Serial.print(F(", "));
    Serial.println(t4);

    dispensing = false;
    for (int i = 0; i < VALVE_COUNT; i++) {
        if (timings[i] > 0) {
            digitalWrite(valves[i], LOW); // enable active-low
            dispensing = true;
        }
    }
}

bool isDispensing(void) {
    return dispensing;
}

int dispensionProgress(void) {
    if (!dispensing) {
        return 0;
    }

    int maxTime = 0;
    for (int i = 0; i < VALVE_COUNT; i++) {
        if (timings[i] > maxTime) {
            maxTime = timings[i];
        }
    }

    unsigned long timeRunning = millis() - startTime;
    uint32_t percent = (uint32_t)timeRunning * 100 / (uint32_t)maxTime;
    return percent;
}

void dispenserThread(void) {
    if (!dispensing) {
        return;
    }

    int running = 0;
    unsigned long time = millis();

    for (int i = 0; i < VALVE_COUNT; i++) {
        if (timings[i] > 0) {
            if (((time - startTime) > timings[i])) {
                if (digitalRead(valves[i]) == LOW) {
                    Serial.print(F("Dispensing on valve "));
                    Serial.print(i + 1);
                    Serial.print(F(" finished after "));
                    Serial.print(time - startTime);
                    Serial.println(F("ms..."));
                }

                // valve is finished
                digitalWrite(valves[i], HIGH); // disable, active-low
            } else {
                // valve is dispensing
                running++;
            }
        }
    }

    if (running == 0) {
        dispensing = false;
    }
}
