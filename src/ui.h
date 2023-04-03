
#pragma once

#include "Arduino.h"
#include <ESPUI.h>
#include "const.h"

void setUpUI();
// void enterWifiDetailsCallback(Control *sender, int type);
void textCallback(Control *sender, int type);
void generalCallback(Control *sender, int type);
void scrambleCallback(Control *sender, int type);
void styleCallback(Control *sender, int type);
void updateCallback(Control *sender, int type);
void getTimeCallback(Control *sender, int type);
void graphAddCallback(Control *sender, int type);
void graphClearCallback(Control *sender, int type);
void randomString(char *buf, int len);
void extendedCallback(Control* sender, int type, void* param);

extern uint16_t traindirectionlabel[2], trainSlider[2], trainPWM[2];
extern uint16_t soundlabel, volSlider;
