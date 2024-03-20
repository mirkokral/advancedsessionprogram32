#include "LuaApp.hpp"
#include "lua/lua.hpp"
#include "lua/lauxlib.h"
bool luaappopen = true;
lv_obj_t* table;
lv_obj_t* input;
lua_State* state = luaL_newstate();

// A function to append a row to lvgl table "table"
void append(const char* s) {
    lv_table_set_cell_value(table, lv_table_get_row_cnt(table), 0, s);
}

//// WRAPPERS
extern "C" {
    int lua_print(lua_State *l) {
        append(lua_tostring(l, 1));
        return 0;
    }
    
}
//// END

void createState() {
    luaL_openlibs(state);
    lua_register(state, "t", lua_print);
}

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
    lv_textarea_set_one_line(input, false);
    lv_obj_set_size(input, 320, 40);
    lv_obj_set_size(table, 320, 240 - 40);
    
    
    
    lv_obj_t* sendbtn = lv_button_create(scr);
    lv_obj_set_size(sendbtn, 40, 40);
    lv_obj_align(sendbtn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_t* sendlbl = lv_label_create(sendbtn);
    // LVGL currently does not have a send icom.
    lv_label_set_text(sendlbl, LV_SYMBOL_UPLOAD);
    

    lv_table_set_column_count(table, 1);
    lv_table_set_cell_value(table, 0, 0, "Lua interpreter");
    append("test");
    lv_obj_add_event_cb(sendbtn, [](lv_event_t* e) {
        // if(lv_indev_get_key(lv_indev_active()) == LV_KEY_ENTER) {
            const char* s = (String(lv_textarea_get_text(input))).c_str();
            // append((">> " + String(lv_textarea_get_text(input))).c_str());
            if((luaL_loadstring(state, s) || lua_pcall(state, 0, LUA_MULTRET, 0))) {
                append("Error running code");
            }
            
            lv_textarea_set_text(input, "");
            lv_group_focus_obj(input);
        // }
    }, LV_EVENT_CLICKED, NULL);
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