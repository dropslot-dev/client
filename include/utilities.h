#ifndef UTILITIES_H
#define UTILITIES_H

#include "Arduino.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

extern esp_chip_info_t chip_info;
void setChipInfo();
const char* getChipModelString(esp_chip_model_t model);
String getTimestamp();

#endif