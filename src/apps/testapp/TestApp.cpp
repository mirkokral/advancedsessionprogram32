#include "TestApp.hpp"
bool appopen = true;
lv_obj_t *TestApp::prepareScreen() {
    lv_obj_t* scr = lv_obj_create(NULL);
    lv_obj_t* label = lv_label_create(scr);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label, "Hello, World!");
    
    return scr;
}

void TestApp::taskFunction( ){
    while(appopen) {
        delay(100);
    }
    appopen=true;
}