#include "LuaApp.hpp"
bool luaappopen = true;
lv_obj_t* table;
lv_obj_t* input;

#define append(item) lv_table_set_cell_value(table, lv_table_get_column_count(table), 0, item); lv_table_set_column_width(table, lv_table_get_column_count(table) - 1, 320)

lv_obj_t *LuaApp::prepareScreen() {
    lv_obj_t* scr = lv_obj_create(NULL);
    table = lv_table_create(scr);
    input = lv_textarea_create(scr);
    // lv_obj_t* consolegroup = lv_obj_create(scr);
    lv_obj_t* closebutton = lv_button_create(scr);
    lv_obj_t* closebuttonlbl = lv_label_create(closebutton);
    lv_obj_align(closebutton, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_event_cb(closebutton, [](lv_event_t* e){
        luaappopen = false;
    }, LV_EVENT_CLICKED, NULL);
    lv_label_set_text(closebuttonlbl, LV_SYMBOL_CLOSE);
    lv_obj_align(table, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_align(input, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_textarea_set_one_line(input, true);
    lv_obj_set_size(input, 320, 40);
    lv_obj_set_size(table, 320, 240 - 40);
    lv_table_set_column_count(table, 1);
    lv_table_set_cell_value(table, 0, 0, "Lua interpreter");
    append("test");
    return scr;
}

void LuaApp::taskFunction( ){
    while(luaappopen) {
        delay(100);
    }
    lv_obj_delete(table);
    lv_obj_delete(input);
    luaappopen=true;
}