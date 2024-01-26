
#include "2048App.hpp"
#include <lv_lib_100ask.h>

bool appopen = true;

void amog(lv_event_t* e) {
    appopen=false;
}

lv_obj_t *Game2048App::prepareScreen() {
    lv_obj_t* scr = lv_obj_create(NULL);
    lv_100ask_2048_create(scr);
    lv_obj_t* exitbtn = lv_button_create(scr);
    lv_obj_t* exitlbl = lv_label_create(exitbtn);
    lv_label_set_text(exitlbl, LV_SYMBOL_LEFT);
    lv_obj_align(exitbtn, LV_ALIGN_TOP_LEFT, -20, 20);
    lv_obj_add_event_cb(exitbtn, amog, LV_EVENT_CLICKED, NULL);
    return scr;
}

void Game2048App::taskFunction( ){
    while(appopen) delay(100);
    appopen=true;
}