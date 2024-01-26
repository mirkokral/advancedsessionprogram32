#pragma once

#include <apps/AppBase.hpp>
#include <Thermal_Printer.h>

class ThermalApp : AppBase {
    public:
        char* icon = LV_SYMBOL_BLUETOOTH;

        char* name = "BT Thermal Printer";
        // Return the screen object to be faded into. If this function returns NULL, then undefined behaviour may happen.
        lv_obj_t* prepareScreen();

        // Task Function. Should not allocate ANY memory whatsoever. Instead make stuff in the class be.
        void taskFunction();

        bool appopen = true;
        String printrpc = "";
        int feedrpc = 0;
        lv_obj_t* scr;
        lv_obj_t* textarea;
        lv_obj_t* closebutton;
        lv_obj_t* closebuttonlbl;
        lv_obj_t* printbutton;
        lv_obj_t* printbuttonlbl;
        lv_obj_t* feedbutton;
        lv_obj_t* feedbuttonlbl;
        static void printbuttonhandler(lv_event_t* e);
        static void feedbuttonhandler(lv_event_t* e);
        static void closebuttonhandler(lv_event_t* e);
};