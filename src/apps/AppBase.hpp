#pragma once

#include <Arduino.h>
#include <lvgl.h>

class AppBase {
    public:
        void* icon;
        char* name;
        TaskHandle_t th = NULL;
        virtual lv_obj_t* prepareScreen();
        virtual void taskFunction();
        

    protected:
    private:
};