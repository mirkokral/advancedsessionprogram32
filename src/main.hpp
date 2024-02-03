#pragma once
#include "esp_heap_caps.h"

#include <lvgl.h>
#include <Arduino.h>
#include <utilities.h>
#include <SPI.h>
#include <LittleFS.h>
#include <WiFi.h>
/*If you want to use the LVGL examples,
  make sure to install the lv_examples Arduino library
  and uncomment the following line.
*/
#include <ui.hpp>
#define TOUCH_MODULES_GT911 1
#include <TouchLib.h>
#include <esp32-hal-psram.h>
#include <LovyanGFX.hpp>
#include <lgfx.h>
#include <SoftwareSerial.h>
static LGFX tft;

void LOG(String what);