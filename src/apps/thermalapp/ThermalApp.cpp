#include "ThermalApp.hpp"
#include <Montserrat_VariableFont_wght17pt7b.h>

// FIXME: Un-shඞඞify this code
// NOTE: It is what it is

void ThermalApp::closebuttonhandler(lv_event_t* e) {
    ((ThermalApp*)(e->user_data))->appopen=false;
}
void ThermalApp::printbuttonhandler(lv_event_t* e) {
    ((ThermalApp*)(e->user_data))->printrpc = String(lv_textarea_get_text(((ThermalApp*)(e->user_data))->textarea)); // 
}
void ThermalApp::feedbuttonhandler(lv_event_t* e) {
    ((ThermalApp*)(e->user_data))->feedrpc += 48;
}

lv_obj_t *ThermalApp::prepareScreen() {
    scr = lv_obj_create(NULL);
    textarea = lv_textarea_create(scr);
    closebutton = lv_button_create(scr);
    closebuttonlbl = lv_label_create(closebutton);
    printbutton = lv_button_create(scr);
    printbuttonlbl = lv_label_create(printbutton);
    feedbutton = lv_button_create(scr);
    feedbuttonlbl = lv_label_create(feedbutton);

    lv_obj_add_event_cb(closebutton, (closebuttonhandler), LV_EVENT_CLICKED, this);
    lv_label_set_text(closebuttonlbl, LV_SYMBOL_CLOSE);
    lv_obj_align(closebutton, LV_ALIGN_TOP_RIGHT, 0, 0);

    lv_obj_set_size(textarea, 320, 240 - lv_obj_get_width(printbutton));
    lv_obj_align(textarea, LV_ALIGN_TOP_MID, 0, 0);


    lv_obj_add_event_cb(printbutton, (printbuttonhandler), LV_EVENT_CLICKED, this);
    lv_label_set_text(printbuttonlbl, LV_SYMBOL_FILE " Print");
    lv_obj_align(printbutton, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(feedbutton, (feedbuttonhandler), LV_EVENT_CLICKED, this);
    lv_label_set_text(feedbuttonlbl, LV_SYMBOL_UPLOAD " Feed");
    lv_obj_align(feedbutton, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    
    return scr;
}

void ThermalApp::taskFunction( ){
    while(appopen) {
        
        if(printrpc != "") {
            lv_obj_remove_flag(printbutton, LV_OBJ_FLAG_CLICKABLE);
            lv_label_set_text(printbuttonlbl, "Prepairing text print.");
            lv_obj_set_state(printbutton, LV_STATE_DISABLED, true);
            String r = printrpc + "";
            r.replace("\n", "\r");
            printrpc = "";
            lv_label_set_text(printbuttonlbl, "Scanning for devices");
            if(tpIsConnected() || tpScan()) {
                lv_label_set_text(printbuttonlbl, "Connecting....");
                
                if(!tpIsConnected()) tpConnect();
                lv_label_set_text(printbuttonlbl, "Printing....");
                // tpSetEnergy(255); // FU*K ECOLOGY I WANT TEXT TEXT
                tpSetWriteMode(MODE_WITHOUT_RESPONSE);
                tpPrintCustomText((GFXfont*)&Font5x7Fixed, 0, (char*)((r + "\r").c_str()));
            } else {
                lv_label_set_text(printbuttonlbl, "Failed to find any device");
                delay(1000);
            }
            // if(!tpIsConnected) {
            //     tpConnect("123:456");
            // }

            lv_label_set_text(printbuttonlbl, "Print");
            lv_textarea_set_text(textarea, "");
            lv_obj_add_flag(printbutton, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_state(printbutton, LV_STATE_DISABLED, false);
        }
        if(feedrpc > 0) {
            if(tpIsConnected()) {
                tpFeed(feedrpc);
                feedrpc = 0;
            }
        }
        delay(100);
    };
    if(tpIsConnected()) tpDisconnect();
    appopen=true;
}