#include "MenuApp.hpp"
#include <WiFi.h>
#include <main.hpp>
bool isappopen = true;
bool scanning = false;

lv_obj_t* scanbtn;
lv_obj_t* constext;
lv_obj_t* wifilist;
lv_obj_t * mbox1;
String lastText = "";
enum {
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2,
    LV_MENU_ITEM_BUILDER_VARIANT_3
};
typedef uint8_t lv_menu_builder_variant_t;
lv_obj_t* root_page;

void wifiScanHandler(lv_event_t* r) {
    scanning = true;
}

void brightnessSliderHandler(lv_event_t* r) {
    tft.setBrightness(lv_slider_get_value(lv_event_get_current_target_obj(r)));
}

static lv_obj_t * create_text(lv_obj_t * parent, const char * icon, const char * txt,
                              lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t * obj = lv_menu_cont_create(parent);

    lv_obj_t * img = NULL;
    lv_obj_t * label = NULL;

    if(icon) {
        img = lv_image_create(obj);
        lv_image_set_src(img, icon);
    }

    if(txt) {
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
        if(builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_3) {
            constext = label;
        }
    }
    if(builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }

    return obj;
}

static lv_obj_t * create_slider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max,
                                int32_t val)
{
    lv_obj_t * obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t * slider = lv_slider_create(obj);
    lv_obj_set_flex_grow(slider, 1);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);

    if(icon == NULL) {
        lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    }

    return obj;
}

static lv_obj_t * create_switch(lv_obj_t * parent, const char * icon, const char * txt, bool chk)
{
    lv_obj_t * obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_t * sw = lv_switch_create(obj);
    lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : 0);

    return obj;
}

TaskHandle_t hhh;

static void back_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = (lv_obj_t*) lv_event_get_target(e);
    lv_obj_t * menu = (lv_obj_t*) lv_event_get_user_data(e);

    if(lv_menu_back_button_is_root(menu, obj)) {
        isappopen = false;
    }
}

lv_obj_t *MenuApp::prepareScreen() {

    

    lv_obj_t* scr = lv_obj_create(NULL);
    lv_obj_t * menu = lv_menu_create(scr);
    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);
    lv_obj_add_event_cb(menu, back_event_handler, LV_EVENT_CLICKED, menu);
    lv_obj_set_size(menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
    lv_obj_center(menu);
    
    lv_obj_t * cont;
    lv_obj_t * section;
    Serial.println("Display Page");
    lv_obj_t * sub_display_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_display_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(sub_display_page);
    Serial.println("Display Section");
    section = lv_menu_section_create(sub_display_page);
    Serial.println("Brightness Slider");
    cont = create_slider(section, LV_SYMBOL_SETTINGS, "Brightness", 40, 255, tft.getBrightness());
    lv_obj_add_event_cb(cont, brightnessSliderHandler, LV_EVENT_VALUE_CHANGED, NULL);


    // Keyboard Page

    Serial.println("Keyboard Page");
    lv_obj_t * sub_kb_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_kb_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(sub_kb_page);
    Serial.println("KB Section");
    section = lv_menu_section_create(sub_kb_page);
    cont = lv_textarea_create(section);
    lv_obj_set_size(cont, LV_PCT(100), 150);
    lv_textarea_set_placeholder_text(cont, "Try out your settings");

    // WiFi Page

    Serial.println("Wifi Page");
    lv_obj_t * sub_wifi_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_wifi_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(sub_wifi_page);
    Serial.println("Wifi Section");
    section = lv_menu_section_create(sub_wifi_page);

    // if(WiFi.isConnected()) {
    create_text(sub_wifi_page, LV_SYMBOL_WIFI, "", LV_MENU_ITEM_BUILDER_VARIANT_3);
    // }

    Serial.println("Scan Button");
    scanbtn = create_text(sub_wifi_page, LV_SYMBOL_REFRESH, "Scan", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_obj_add_event_cb(scanbtn, wifiScanHandler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(scanbtn, LV_OBJ_FLAG_CLICKABLE);

    Serial.println("Wifi List");
    wifilist = lv_list_create(sub_wifi_page);
    lv_obj_clean(wifilist);
    Serial.println("Add text that not scanned yet");
    lv_list_add_text(wifilist, "Not scanned yet.");
    lv_obj_set_size(wifilist, LV_PCT(100), 150);
    

    // ROOT PAGE
    Serial.println("root_page = lv_menu_page_create(menu, \"Settings\");");
    root_page = lv_menu_page_create(menu, "Settings");
    Serial.println("lv_obj_set_style_pad_hor(root_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);");
    lv_obj_set_style_pad_hor(root_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    Serial.println("Root Page section");
    section = lv_menu_section_create(root_page);
    Serial.println("Wifi section button");
    cont = create_text(section, LV_SYMBOL_WIFI, "WiFi", LV_MENU_ITEM_BUILDER_VARIANT_1);
    Serial.println("lv_menu_set_load_page_event(menu, cont, sub_wifi_page);");
    lv_menu_set_load_page_event(menu, cont, sub_wifi_page);
    Serial.println("Keyboard section button");
    cont = create_text(section, LV_SYMBOL_KEYBOARD, "Keyboard", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_kb_page);
    Serial.println("Display section button");
    cont = create_text(section, "", "Display", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_display_page);

    Serial.println("Set Sidebar Page");
    lv_menu_set_sidebar_page(menu, root_page);

    Serial.println("Some goofy ahh random event");
    lv_obj_send_event(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED,
                      NULL);


    return scr;
}

// Stole this directly from the android source code :trol:
int calculateSignalLevel(int rssi, int numLevels) {
    if (rssi <= -100) {
        return 0;
    } else if (rssi >= -55) {
        return numLevels - 1;
    } else {
        float inputRange = (-55 - -100);
        float outputRange = (numLevels - 1);
        return (int)((float)(rssi - -100) * outputRange / inputRange);
    }
}

lv_obj_t* passwordprompt;
lv_obj_t* mbox;

void wificonnectevthndlr(lv_event_t* e) {
    int i = *(int*)(e->user_data);
    String ssid = WiFi.SSID(i);
    Serial.println(ssid);
    String password = String(lv_textarea_get_text(passwordprompt));
    WiFi.begin(ssid, password);
    auto result = WiFi.waitForConnectResult();
    lv_msgbox_close(mbox);
}

void wificonnect(lv_event_t* e) {
    int i = *(int*)(e->user_data);
    String ssid;
    uint8_t encryptiontype;
    int32_t rssi;
    uint8_t* bssid;
    int32_t channel;
    
    bool result = WiFi.getNetworkInfo(i, ssid, encryptiontype, rssi, bssid, channel);
    Serial.printf("Connecting to %s. Result is %s. Index: %i", ssid, result ? "true" : "false", i);
    Serial.flush();
    if(encryptiontype == WIFI_AUTH_OPEN) {
        if(WiFi.isConnected()) WiFi.disconnect();
        WiFi.begin(ssid);
    } else {
        mbox = lv_msgbox_create(NULL);
        lv_msgbox_add_title(mbox, "Password");
        lv_msgbox_add_close_button(mbox);
        passwordprompt = lv_textarea_create(mbox);
        lv_textarea_set_one_line(passwordprompt, true);
        lv_textarea_set_password_mode(passwordprompt, true);
        lv_obj_add_event_cb(lv_msgbox_add_footer_button(mbox, "Connect"), wificonnectevthndlr, LV_EVENT_CLICKED, e->user_data);
        
    }
}

void MenuApp::taskFunction( ){
    hhh=xTaskGetIdleTaskHandle();
    while(isappopen) {
        if(scanning) {
            // WiFi.scanDelete();
            lv_obj_clean(wifilist);
            lv_list_add_text(wifilist, "Scanning...");
            WiFi.scanNetworks(true);
            int n = WiFi.scanComplete();
            while(n == -1) {
                if(!isappopen) { scanning=false; isappopen=true; return; };
                n = WiFi.scanComplete();
                delay(1000);
            }
            lv_obj_clean(wifilist);
            for(int i = 0; i < n; i++) {
                Serial.printf("%i: %s", i, WiFi.SSID(i));
                Serial.flush();
                int level = calculateSignalLevel(WiFi.RSSI(i), 5);
                String lvltext = "";

                switch(level) {
                    case 0:
                        lvltext = "----";
                        break;
                    case 1:
                        lvltext = "o---";
                        break;
                    case 2:
                        lvltext = "oo--";
                        break;
                    case 3:
                        lvltext = "ooo-";
                        break;
                    case 4:
                        lvltext = "oooo";
                        break;

                    default:
                        lvltext = "errr";
                        break;
                }
                int amogus = i + 1 -1 ;
                lv_obj_t *btn = lv_list_add_btn(wifilist, LV_SYMBOL_WIFI, (WiFi.SSID(i) + String(" (") + lvltext + String(")")).c_str());
                lv_obj_add_event_cb(btn, wificonnect, LV_EVENT_CLICKED, &amogus);
            }
            scanning = false;
        }
        if(WiFi.isConnected()) {
            String text = (
                String("Connected to ") +
                WiFi.SSID() +
                String(" with IP ") +
                WiFi.localIP().toString() +
                String(",")
            );
            if(text != lastText) {
                lv_label_set_text(constext, text.c_str());
                lastText = text;
            }
        } else {
            lv_label_set_text(constext, "Disconnected.");
        }
        delay(100);
    }
    isappopen = true;
}