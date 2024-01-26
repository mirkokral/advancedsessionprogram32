#include "ui.hpp"

#include <ArrayList.h>
#include "wallpaper.c"

#include <apps/AppBase.hpp>
#include <apps/testapp/TestApp.hpp>
#include <apps/menuexapp/MenuApp.hpp>
#include <apps/thermalapp/ThermalApp.hpp>
#include <apps/2048app/2048App.hpp>


// GROUPS
static lv_group_t* gr;
static lv_group_t * g;

// OBJECTS
static lv_obj_t * mainscr;
static lv_obj_t * applist;
static lv_obj_t * clocklabel;
static lv_obj_t * controlframe;
static lv_obj_t * wifibtn;
static lv_obj_t * lockbtn;
static lv_obj_t * unlockbtn;
static lv_obj_t * brightnessbar;
static lv_obj_t * pWifi;
static lv_obj_t * iWifi;
static lv_obj_t * bWifiClose;
static lv_obj_t * lWifiClose;
static lv_obj_t * bWifiScan;
static lv_obj_t * lWifiScan;

// STATE VARIABLES
bool locked = true;
AppBase* currentlyRunningApp = NULL;

// UTILITY VARIABLES
lv_obj_t* menuitems[] = {wifibtn, lockbtn, applist}; // Menu items that need to be unclickable while not shown
lv_obj_t* wifidialogitems[] = {iWifi, bWifiClose}; // Wifi dialog items that need to be unclickable while not shown


// FUNCTION DEFINITIONS
static void updateLocked();

bool isapphandleropen = false;
// void appcrashhandler(void * _) {
//     isapphandleropen = true;
//     for(;;) {
//         if(currentlyRunningApp != NULL) {
//             if(currentlyRunningApp->th != NULL) {
//                 eTaskState r = eTaskGetState(currentlyRunningApp->th);
//                 switch(r) {
//                     case eReady:
//                     case eRunning:
//                     case 2:
//                         break;

//                     default:
//                         Serial.println(r);
//                         vTaskDelete(currentlyRunningApp->th);
//                         exitApp();
//                 }
//             }
//         }
//         delay(1000);
//     }
// }

void AppTH(void* app) {
    ((AppBase*)app)->th = xTaskGetCurrentTaskHandle();
    ((AppBase*)app)->taskFunction();
    lv_screen_load_anim(
        mainscr,
        LV_SCR_LOAD_ANIM_FADE_ON,
        100,
        0,
        true
    );
    while(lv_screen_active() != mainscr) delay(100);
    currentlyRunningApp = NULL;
    vTaskDelete(xTaskGetCurrentTaskHandle());
}

void RunApp(AppBase* app) {
    // if(!isapphandleropen) {
        // xTaskCreate(
        //     appcrashhandler,
        //     "App Crash Handler",
        //     16667,
        //     NULL,
        //     0,
        //     NULL
        // );
    // }
    if(currentlyRunningApp == NULL) {
        currentlyRunningApp = app;
        lv_obj_t* nscr = app->prepareScreen();

        lv_screen_load_anim(
            nscr,
            LV_SCR_LOAD_ANIM_FADE_ON,
            100,
            0,
            false
        );
        xTaskCreate(AppTH, (String(app->name) + " task").c_str(), 16667, (void*)app, 0, NULL);
    }
    
}
void mainuiclickable(bool clickable);

static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_align(((lv_obj_t*)var), LV_ALIGN_LEFT_MID, v, 0);
}
static void anim_xr_cb(void * var, int32_t v)
{
    lv_obj_align(((lv_obj_t*)var), LV_ALIGN_RIGHT_MID, v, 0);
}
static void anim_y_cb(void * var, int32_t v)
{
    lv_obj_align(((lv_obj_t*)var), LV_ALIGN_BOTTOM_MID, 0, v);
}
static void anim_winopen_cb(void * var, int32_t v)
{
    lv_obj_set_style_opa((lv_obj_t*)var, v, LV_PART_ANY);
    lv_obj_set_style_transform_scale((lv_obj_t*)var, v, LV_PART_MAIN);
    lv_obj_align((lv_obj_t*)var, LV_ALIGN_CENTER,0 ,0);
}
static void event_handler(lv_event_t * e)
{
    if(e->current_target == lockbtn) {
        switch(e->code) {
            case LV_EVENT_CLICKED:
                locked = true;
                
                updateLocked();
                break;
            default:
                break;
        }
    }
    if(e->current_target == unlockbtn) {

        switch(e->code) {
            case LV_EVENT_CLICKED:
                locked = false;
                updateLocked();
                break;
            default:
                break;
        }
    }
    if(e->current_target == wifibtn) {

        switch(e->code) {
            case LV_EVENT_CLICKED:
                Serial.println("wifibtn: Clicked");
                lv_anim_t openanim;
                Serial.println("anim init");
                lv_anim_init(&openanim);
                lv_anim_set_var(&openanim, pWifi);
                lv_anim_set_values(&openanim, LV_OPA_0, LV_OPA_100);
                lv_anim_set_time(&openanim, 2000);
                lv_anim_set_exec_cb(&openanim, anim_winopen_cb);
                lv_anim_set_path_cb(&openanim, lv_anim_path_linear);
                lv_anim_start(&openanim);
                break;
            default:
                break;
        }
    }
    if(e->current_target == bWifiClose) {

        switch(e->code) {
            case LV_EVENT_CLICKED:
                Serial.println("bWifiClose: Clicked");
                lv_anim_t closeanim;
                Serial.println("animinit");
                lv_anim_init(&closeanim);
                Serial.println("varset");
                lv_anim_set_var(&closeanim, pWifi);
                Serial.println("valueset");
                lv_anim_set_values(&closeanim, LV_OPA_100, LV_OPA_0);
                Serial.println("timeset");
                lv_anim_set_time(&closeanim, 2000);
                Serial.println("execcbset");
                lv_anim_set_exec_cb(&closeanim, anim_winopen_cb);
                Serial.println("pathcbset");
                lv_anim_set_path_cb(&closeanim, lv_anim_path_linear);
                Serial.println("startcbset");
                lv_anim_start(&closeanim);
                Serial.println("started & done");
                break;
            default:
                break;
        }
    }
}

void click(lv_event_t* e) {

    if(!locked) {
        RunApp((AppBase*)e->user_data);
    }
};

void updateLocked() {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, controlframe);
    if (locked) {
        lv_anim_set_values(&a, lv_obj_get_x_aligned(controlframe), -200);
    } else {
        lv_anim_set_values(&a, lv_obj_get_x_aligned(controlframe), 0);
    }
    lv_anim_set_time(&a, 1000);
    lv_anim_set_exec_cb(&a, anim_x_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);

    lv_anim_t b;
    lv_anim_init(&b);
    lv_anim_set_var(&b, applist);
    if (locked) {
        lv_anim_set_values(&b, lv_obj_get_x_aligned(applist), 200);
    } else {
        lv_anim_set_values(&b, lv_obj_get_x_aligned(applist), 0);
    }
    lv_anim_set_time(&b, 1000);
    lv_anim_set_exec_cb(&b, anim_xr_cb);
    lv_anim_set_path_cb(&b, lv_anim_path_ease_in_out);
    lv_anim_t c;
    lv_anim_init(&c);
    lv_anim_set_var(&c, unlockbtn);
    lv_obj_add_flag(unlockbtn, LV_OBJ_FLAG_CLICKABLE);
    if (locked) {
        lv_anim_set_values(&c, lv_obj_get_y_aligned(unlockbtn), -20);
    } else {
        lv_anim_set_values(&c, lv_obj_get_y_aligned(unlockbtn), 60);
        
    }
    lv_anim_set_time(&c, 1000);
    lv_anim_set_exec_cb(&c, anim_y_cb);
    lv_anim_set_path_cb(&c, lv_anim_path_ease_in_out);
    lv_anim_start(&a);
    lv_anim_start(&b);
    lv_anim_start(&c);
}

void ui_init() {
    
    g = lv_group_create();
    lv_group_set_default(g);

    lv_indev_t * indev = NULL;
    for(;;) {
        indev = lv_indev_get_next(indev);
        if(!indev) {
            break;
        }

        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if(indev_type == LV_INDEV_TYPE_KEYPAD) {
            lv_indev_set_group(indev, g);
        }

        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            lv_indev_set_group(indev, g);
        }
    }
    mainscr = lv_obj_create(NULL);
    lv_screen_load(mainscr);
    // Create the wallpaper
    lv_obj_t *wp = lv_img_create(mainscr);
    // LV_IMG_DECLARE(wallpaper);
    lv_img_set_src(wp, &wallpaper);
    lv_obj_align(wp, LV_ALIGN_CENTER, 0, 0);
    // Create the clock
    
    clocklabel = lv_label_create(mainscr);
    lv_obj_align(clocklabel, LV_ALIGN_CENTER, 0, -40);
    lv_label_set_text(clocklabel, "10:30");
    lv_obj_set_style_text_font(clocklabel, &lv_font_montserrat_48, LV_STATE_ANY);

    gr = lv_group_create();
    /*Create a list*/
    applist = lv_list_create(mainscr);
    lv_obj_set_size(applist, 130, 240);
    lv_obj_align(applist, LV_ALIGN_RIGHT_MID, 200, 0);
    lv_obj_remove_flag(mainscr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(applist, event_handler, LV_EVENT_ALL, NULL);

    lv_obj_t* tappbtn;
    
    
    TestApp* testapp = new TestApp();
    tappbtn = lv_list_add_button(applist, (testapp->icon), (testapp->name));
    lv_obj_add_event_cb(tappbtn, click, LV_EVENT_CLICKED, testapp);
    lv_obj_set_style_bg_opa(tappbtn, 0, LV_PART_MAIN);

    ThermalApp* thermapp = new ThermalApp();
    tappbtn = lv_list_add_button(applist, (thermapp->icon), (thermapp->name));
    lv_obj_add_event_cb(tappbtn, click, LV_EVENT_CLICKED, thermapp);
    lv_obj_set_style_bg_opa(tappbtn, 0, LV_PART_MAIN);

    MenuApp* menuapp = new MenuApp();
    tappbtn = lv_list_add_button(applist, (menuapp->icon), (menuapp->name));
    lv_obj_add_event_cb(tappbtn, click, LV_EVENT_CLICKED, menuapp);
    lv_obj_set_style_bg_opa(tappbtn, 0, LV_PART_MAIN);

    Game2048App* g2048app = new Game2048App();
    tappbtn = lv_list_add_button(applist, (g2048app->icon), (g2048app->name));
    lv_obj_add_event_cb(tappbtn, click, LV_EVENT_CLICKED, g2048app);
    lv_obj_set_style_bg_opa(tappbtn, 0, LV_PART_MAIN);

    lv_obj_set_style_border_width(applist, 0, LV_PART_MAIN);
    lv_obj_set_style_opa(applist, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_radius(applist, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(applist, 75, LV_PART_MAIN);


    controlframe = lv_obj_create(mainscr);
    lv_obj_set_size(controlframe, 130, 240);
    lv_obj_align(controlframe, LV_ALIGN_LEFT_MID, -200, 0);
    lv_obj_set_style_border_width(controlframe, 0, LV_PART_MAIN);
    lv_obj_set_style_opa(controlframe, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_radius(controlframe, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(controlframe, 75, LV_PART_MAIN);
    lv_obj_set_flex_align(controlframe, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_flex_flow(controlframe, LV_FLEX_FLOW_COLUMN);



    wifibtn = lv_btn_create(NULL);
    lv_obj_t * wifilbl = lv_label_create(wifibtn);
    lv_label_set_text(wifilbl, "WiFi");
    lv_obj_align(wifilbl, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(wifibtn, LV_COORD_GET_PCT(100), 40);
    lv_obj_add_event_cb(wifibtn, event_handler, LV_EVENT_ALL, NULL);

    lockbtn = lv_btn_create(controlframe);
    lv_obj_t * locklbl = lv_label_create(lockbtn);
    lv_label_set_text(locklbl, "Lock");
    lv_obj_align(locklbl, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(lockbtn, LV_COORD_GET_PCT(100), 40);
    lv_obj_add_event_cb(lockbtn, event_handler, LV_EVENT_ALL, NULL);

    unlockbtn = lv_btn_create(mainscr);
    lv_obj_t * unlocklbl = lv_label_create(unlockbtn);
    lv_label_set_text(unlocklbl, LV_SYMBOL_HOME);
    lv_obj_align(unlocklbl, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(unlockbtn, LV_COORD_GET_PCT(100), 40);
    lv_obj_add_event_cb(unlockbtn, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(unlockbtn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_bg_opa(unlockbtn, 0, LV_PART_MAIN);

    pWifi = lv_obj_create(mainscr);
    lv_obj_set_size(pWifi, 320, 240);
    lv_obj_set_style_transform_scale(pWifi, 0, LV_PART_MAIN);

    iWifi = lv_list_create(pWifi);
    bWifiClose = lv_btn_create(pWifi);
    lWifiClose = lv_label_create(bWifiClose);
    lv_obj_align(bWifiClose, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_align(lWifiClose, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(lWifiClose, LV_SYMBOL_CLOSE);
    lv_obj_set_size(bWifiClose, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(bWifiClose, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(bWifiClose, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(pWifi, LV_ALIGN_CENTER,0 ,0);
    lv_obj_align(iWifi, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_size(iWifi, LV_PCT(100), LV_PCT(100));

    updateLocked();
}

void setCurrentlyRunningApp(AppBase app)
{
    currentlyRunningApp = &app;
}


