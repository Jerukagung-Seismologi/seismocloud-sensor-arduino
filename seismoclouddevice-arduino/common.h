
#ifndef __COMMON_H
#define __COMMON_H

#include <Ethernet.h>
#include <EEPROM.h>

#include "TrueRandom.h"
#include "SoftReset.h"
#include "AcceleroMMA7361.h"
#include "ntp.h"
#include "utils.h"
#include "httpclient.h"
#include "CommandInterface.h"
#include "seismometer.h"

void setUuidNumber(byte* uuid);
byte* getUuidNumber();
float getLatitude();
float getLongitude();
unsigned long getBootTime();
void setBootTime(unsigned long);
void setLatitude(float l);
void setLongitude(float l);
void loadConfig();
void setProbeSpeedStatistic(uint32_t);
uint32_t getProbeSpeedStatistic();

#endif
