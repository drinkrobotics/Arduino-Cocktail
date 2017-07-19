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

#ifndef __DISPENSER_H__
#define __DISPENSER_H__

// Pin assignments for relais / solenoid valves
#define V1_OUTPUT 2
#define V2_OUTPUT 3
#define V3_OUTPUT 4
#define V4_OUTPUT 5

#define VALVE_COUNT 4

void initDispenser(void);
void startDispensing(int t1, int t2, int t3, int t4);
bool isDispensing(void);
int dispensionProgress(void);
void dispenserThread(void);

extern int valves[VALVE_COUNT];

#endif // __DISPENSER_H__

