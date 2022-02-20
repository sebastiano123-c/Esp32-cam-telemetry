#pragma once

#include "FS.h" 
#include "SD_MMC.h" 
#include "telemetry.h"
#include "time.h"

extern uint8_t isConnectedSD;

void setupSD();
void writeDataLogFlight(fs::FS &fs);
void updateConfigFile(fs::FS &fs);
