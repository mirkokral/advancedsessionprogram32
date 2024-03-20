#include "FileApp.hpp"
#include <LittleFS.h>
bool appopenfapp = true;

String path = "/";
lv_obj_t *scr;
lv_obj_t *label; 
lv_obj_t *list;

void update() {
    fs::File d = LittleFS.open(path);
    if(d.isDirectory()) {
        lv_obj_clean(list);
        lv_obj_remove_flag(list, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
        
        lv_list_add_btn(list, LV_SYMBOL_BACKSPACE, "..");
        auto n = String();
        while(n = d.getNextFileName()) {
            lv_list_add_button(list, LV_SYMBOL_DIRECTORY, n.c_str());
        }
        lv_list_add_btn(list, LV_SYMBOL_DIRECTORY, path.c_str());

    } else {
        lv_obj_remove_flag(label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(list, LV_OBJ_FLAG_HIDDEN);        
        lv_label_set_text(label, d.readString().c_str());
    }
}

lv_obj_t *FileApp::prepareScreen() {
    scr = lv_obj_create(NULL);
    label = lv_label_create(scr);
    list = lv_list_create(scr);
    lv_obj_t* closebutton = lv_button_create(scr);
    lv_obj_align(closebutton, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_t* closebuttonlbl = lv_label_create(closebutton);
    lv_obj_add_event_cb(closebutton, [](lv_event_t* e){
        appopenfapp = false;
    }, LV_EVENT_CLICKED, NULL);
    lv_label_set_text(closebuttonlbl, LV_SYMBOL_CLOSE);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(list, LV_OBJ_FLAG_HIDDEN);
    update();
    return scr;
}

void FileApp::taskFunction( ){
    while(appopenfapp) {
        delay(100);
    }
    appopenfapp=true;
}