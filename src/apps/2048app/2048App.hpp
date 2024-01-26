#pragma once

#include <apps/AppBase.hpp>

class Game2048App : AppBase {
    public:
        char* icon = LV_SYMBOL_BARS;

        char* name = "2048";
        // Return the screen object to be faded into. If this function returns NULL, then the app will not be ran.
        lv_obj_t* prepareScreen();

        // Task Function. Should not allocate ANY memory whatsoever. Instead make stuff in the class be.
        void taskFunction();
};