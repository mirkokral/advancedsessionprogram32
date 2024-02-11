#include "LuaApp.hpp"
bool luaappopen = true;
lv_obj_t *LuaApp::prepareScreen() {
    lv_obj_t* scr = lv_obj_create(NULL);
    lv_obj_t* consolegroup = lv_obj_create(scr);
    lv_obj_t* closebutton = lv_button_create(scr);
    lv_obj_t* closebuttonlbl = lv_label_create(closebutton);
    lv_obj_align(closebutton, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_event_cb(closebutton, [](lv_event_t* e){
        luaappopen = false;
    }, LV_EVENT_CLICKED, NULL);
    lv_label_set_text(closebuttonlbl, LV_SYMBOL_CLOSE);
    lv_obj_set_size(consolegroup, 320, 240);
    lv_obj_set_style_radius(consolegroup, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(consolegroup, 0, LV_PART_MAIN);
    lv_obj_set_flex_align(consolegroup, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_flex_flow(consolegroup, LV_FLEX_FLOW_COLUMN);
    lv_label_set_text(lv_label_create(consolegroup), "Test 1");
    lv_label_set_text(lv_label_create(consolegroup), "Test 2");
    lv_label_set_text(lv_label_create(consolegroup), "Test 3");

    return scr;
}

void LuaApp::taskFunction( ){
    while(luaappopen) {
        delay(100);
    }
    luaappopen=true;
}