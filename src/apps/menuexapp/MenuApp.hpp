#pragma once

#include <apps/AppBase.hpp>
#include <settings.hpp>

enum
{
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2,
    LV_MENU_ITEM_BUILDER_VARIANT_3
};
typedef uint8_t lv_menu_builder_variant_t;
class MenuApp : AppBase
{
public:
    char *icon = LV_SYMBOL_SETTINGS;

    char *name = "Settings";
    // Return the screen object to be faded into. If this function returns NULL, then the app will not be ran.
    lv_obj_t *prepareScreen();

    // Task Function. Should not allocate ANY memory whatsoever. Instead make stuff in the class be.
    void taskFunction();

private:
    lv_obj_t *scanbtn;
    lv_obj_t *constext;
    lv_obj_t *wifilist;
    lv_obj_t *mbox1;
    lv_obj_t *root_page;
    lv_obj_t *create_text(lv_obj_t *parent, const char *icon, const char *txt,
                          lv_menu_builder_variant_t builder_variant);
    lv_obj_t *create_slider(lv_obj_t *parent, const char *icon, const char *txt, int32_t min, int32_t max,
                            int32_t val);
    lv_obj_t *create_switch(lv_obj_t *parent, const char *icon, const char *txt, bool chk);
    lv_obj_t *MenuApp::create_dropdown(lv_obj_t *parent, const char *icon, const char *txt, char *choices[], int current);
};