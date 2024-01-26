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
static LGFX tft;

String printqueue = "";
char* sprbuf = "";


#define LOGP(what, ...) \
  sprintf(sprbuf, String(what).c_str(), __VA_ARGS__);\
  printqueue += String(sprbuf);\
  Serial.print(sprbuf);\
  Serial.flush();
  

#define LOG(what) \
  printqueue += String(what) + "\n";\
  Serial.println(what);

#define pqd tft.printf(printqueue.c_str())

#define PANIC(reason, whattodo) \ 
    tft.println("Panic!");\
    tft.printf("At: %s:%d\n", __FILE__, __LINE__);\
    tft.println("");\
    tft.println(String("Reason: ") + String(reason) + String("\n"));\
    tft.println(String("To fix, try ") + String(whattodo) + String("\n"));\
    while(1) {\
        delay(1000);\ 
    }

