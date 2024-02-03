#include "rsi.hpp"
#include <main.hpp>
void rEnter(String reason) {
    lv_screen_load(lv_obj_create(NULL));
    // lv_obj_t* label = lv_label_create(lv_screen_active());
    // lv_label_set_text(label, "[RSI mode]");
    // lv_obj_align(label, LV_ALIGN_TOP_RIGHT, 0, 0);

    /*Create a menu object*/
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
    lv_obj_center(menu);

    /*Modify the header*/
    lv_obj_t * back_btn = lv_menu_get_main_header_back_button(menu);
    lv_obj_t * back_button_label = lv_label_create(back_btn);
    lv_label_set_text(back_button_label, "Back");

    lv_obj_t * cont;
    lv_obj_t * label;

    /*Create sub pages*/
    lv_obj_t * sub_1_page = lv_menu_page_create(menu, "Reboot");

    cont = lv_menu_cont_create(sub_1_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "System");
    lv_obj_add_event_cb(cont, [](lv_event_t* e){
        ESP.restart();
    }, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    cont = lv_menu_cont_create(sub_1_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Bootloader");
    lv_obj_add_event_cb(cont, [](lv_event_t* e){
        
        ESP.restart();
    }, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);

    
    /*Create a main page*/
    lv_obj_t * main_page = lv_menu_page_create(menu, "RSI menu");

    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Reboot");
    lv_menu_set_load_page_event(menu, cont, sub_1_page);


    lv_menu_set_page(menu, main_page);

    while(1) {
        unsigned long d = millis();
        // tft.setCursor(0, 0);
        delay(lv_timer_handler());
        lv_tick_inc(millis() - d);
        
    }
}