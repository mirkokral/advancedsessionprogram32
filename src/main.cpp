#include <Arduino.h>
#include <SoftwareSerial.h>
#include <config.h>
#include <settings.hpp>

#if 1
    SoftwareSerial ss(3, 1);
    #define Serial ss // Still be able to access serial using the grove port on the side
#endif

#include <main.hpp>
#include <rsi.hpp>

// SoftwareSerial* loggingStream;

// #define Serial (*loggingStream) // Still be able to access serial using the grove port on the side

unsigned long m = 0;
/*Change to your screen resolution*/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;
TouchLib *touch = NULL;

String printqueue = "";
char* sprbuf = "";


#include "esp_task_wdt.h"
// #include <rtc_wdt.h>


// lv_obj_t *list;
// lv_obj_t *label;
// TouchLib *tl;
// bool kbd = false;
// static lv_draw_buf_t * buf = lv_draw_buf_create(320, 240, LV_COLOR_FORMAT_RGB565A8, 0);
// static lv_draw_buf_t * draw_buf2;
// static lv_color_t buf2[ screenWidth * screenHeight / 10 ];


void LOG(String what) {
  printqueue = printqueue + String(what) + "\n";
  Serial.println(what);
}

#define LOGP(what, ...)\
  sprintf(sprbuf, String(what).c_str(), __VA_ARGS__);\
  LOG(String(sprbuf));

  

#define pqd printqueue = ""

#define PANIC(reason, whattodo) \
    tft.setColor<uint16_t>(0xf800);\
    tft.setCursor(0, 0); \
    tft.println("Panic!");\
    tft.printf("At: %s:%d\n", __FILE__, __LINE__);\
    tft.println("");\
    tft.println(String("Reason: ") + String(reason) + String("\n"));\
    tft.println(String("To fix, try ") + String(whattodo) + String("\n"));\
    while(1) {\
        delay(1000);\
    }

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char *buf)
{
    Serial.printf("[LVGL] ");
    switch (level)
    {
    case LV_LOG_LEVEL_WARN:
        Serial.printf("WARN: ");
        break;
    case LV_LOG_LEVEL_ERROR:
        Serial.printf("ERROR: ");
        break;
    case LV_LOG_LEVEL_INFO:
        Serial.printf("INFO: ");
        break;
    case LV_LOG_LEVEL_NONE:
        Serial.printf("INV: ");
        break;
    case LV_LOG_LEVEL_TRACE:
        Serial.printf("TRACE: ");
        break;
    case LV_LOG_LEVEL_USER:
        Serial.printf("USER: ");
        break;
    default:
        Serial.printf("INV: ");
        break;
    }
    Serial.printf(buf);
    Serial.flush();
}
#endif
union RGBCOL
{
    struct
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } color;
    uint32_t rescol;
};
static bool getTouch(int32_t &x, int32_t &y)
{
    uint8_t rotation = tft.getRotation();
    if (!touch->read()) {
        return false;
    }
    TP_Point t = touch->getPoint(0);
    switch (rotation) {
    case 1:
        x = t.y;
        y = tft.height() - t.x;
        break;
    case 2:
        x = tft.width() - t.x;
        y = tft.height() - t.y;
        break;
    case 3:
        x = tft.width() - t.y;
        y = t.x;
        break;
    case 0:
    default:
        x = t.x;
        y = t.y;
    }
    Serial.printf("R:%d X:%d Y:%d\n", rotation, x, y);
    return true;
}
/* Display flushing */
void my_disp_flush(lv_display_t * disp, const lv_area_t * area, lv_color16_t * px_map)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    #if PUSHPIXELS_USE_DMA
    tft.pushPixelsDMA((uint16_t *)px_map, w * h, true); // Each pixel takes 2 bytes (16 bits)
    #else
    tft.pushPixels((uint16_t*)px_map, w*h, true);
    #endif
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

static uint32_t last_key = 0;
uint32_t act_key;

lv_indev_t *kb_indev = NULL;
lv_indev_t *tb_indev = NULL;

// Read key value from esp32c3
static uint32_t keypad_get_key_temp(void)
{
    char key_ch = 0;
    Wire.requestFrom(0x55, 1);
    while (Wire.available() > 0)
    {
        key_ch = Wire.read();
    }
    return key_ch;
}
static uint32_t keypad_get_key(void) {
    uint32_t key = keypad_get_key_temp();
    if(key == 0x0C) {
        // Alt+C chord
        uint32_t chord = keypad_get_key_temp();
        while(chord == 0) {
            chord = keypad_get_key_temp();
            delay(50);
        }
        switch (chord)
        {
        case ' ': return '\t'; break;
        case 'q': return '~'; break;
        case 'w': return '%'; break;
        case 'e': return '|'; break;
        case 'r': return '%'; break;
        case 't': return '{'; break;
        case 'y': return '}'; break;
        case '(': return '['; break;
        case ')': return ']'; break;
        case 'u': return '^'; break;
        case 'i': return '<'; break;
        case 'o': return '>'; break;
        case 'p': return '='; break;
        case 'g': return '\\'; break;
        case 'k': return '~'; break;
        // stole this directly outta tulipcc docs :tr:
        default:
            return chord;
            break;
        }
    } else {
        return key;
    }
}

/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_t *indev_drv, lv_indev_data_t *data)
{
    last_key = 0;
    act_key = keypad_get_key();
    if (act_key != 0)
    {
        data->state = LV_INDEV_STATE_PR;
        Serial.printf("Key pressed : 0x%x\r\n", act_key);
        last_key = act_key;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
    data->key = last_key;
}
static uint32_t lki = 0;
bool enterpressed = false;
/*Will be called by the library to read the mouse*/
static void trackball_read(lv_indev_t *indev_drv, lv_indev_data_t *data)
{

    const uint8_t dir_pins[5] = {BOARD_TBOX_G02,
                                 BOARD_TBOX_G01,
                                 BOARD_TBOX_G04,
                                 BOARD_TBOX_G03,
                                 BOARD_BOOT_PIN};
    static bool last_dir[5];
    // uint8_t pos = 10;
    int fi = 5;
    for (int i = 0; i < 5; i++)
    {
        bool dir = digitalRead(dir_pins[i]);
        if (dir != last_dir[i])
        {
            last_dir[i] = dir;
            fi = i;
            break;
        }
    }

    switch (fi)
    {
    case 0:
        if (uSettings.trackball_mode == TRACKBALL_MODE_ARROW) {
            lki = LV_KEY_RIGHT;
            data->key = LV_KEY_RIGHT;
            data->state = LV_INDEV_STATE_PR;
        }
        break;
    case 1:
        switch(uSettings.trackball_mode) {
            case TRACKBALL_MODE_NONE:
            case TRACKBALL_MODE_CURSOR:
                break;
            case TRACKBALL_MODE_NAVIGATE:
                lki = LV_KEY_PREV;
                data->key = LV_KEY_PREV;
                data->state = LV_INDEV_STATE_PR;
                break;
            case TRACKBALL_MODE_ARROW:
                lki = LV_KEY_UP;
                data->key = LV_KEY_UP;
                data->state = LV_INDEV_STATE_PR;
                break;
        }
        break;
    case 2:
        if(uSettings.trackball_mode == TRACKBALL_MODE_ARROW) {

            lki = LV_KEY_LEFT;
            data->key = LV_KEY_LEFT;
            data->state = LV_INDEV_STATE_PR;
        }
        break;
    case 3:
        switch(uSettings.trackball_mode) {
            case TRACKBALL_MODE_NONE:
            case TRACKBALL_MODE_CURSOR:
                break;
            case TRACKBALL_MODE_NAVIGATE:
                lki = LV_KEY_NEXT;
                data->key = LV_KEY_NEXT;
                data->state = LV_INDEV_STATE_PR;
                break;
            case TRACKBALL_MODE_ARROW:
                lki = LV_KEY_DOWN;
                data->key = LV_KEY_DOWN;
                data->state = LV_INDEV_STATE_PR;
                break;
        }
        break;
    case 4:
        // lki = LV_KEY_ENTER;
        enterpressed = !enterpressed;
        data->key = LV_KEY_ENTER;
        data->state = enterpressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
        break;
    default:
        data->key = lki;
        data->state = LV_INDEV_STATE_REL;
        break;
    }
}
/*Read the touchpad*/
void my_touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data)
{

    data->state = getTouch(data->point.x, data->point.y) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}
uint8_t touchAddress = GT911_SLAVE_ADDRESS2;
void scanDevices(TwoWire *w)
{
    uint8_t err, addr;
    int nDevices = 0;
    uint32_t start = 0;
    for (addr = 1; addr < 127; addr++)
    {
        start = millis();
        w->beginTransmission(addr);
        delay(2);
        err = w->endTransmission();
        if (err == 0)
        {
            nDevices++;
            Serial.print("I2C device found at address 0x");
            if (addr < 16)
            {
                Serial.print("0");
            }
            Serial.print(addr, HEX);
            Serial.println(" !");

            if (addr == GT911_SLAVE_ADDRESS2)
            {
                touchAddress = GT911_SLAVE_ADDRESS2;
                Serial.println("Find GT911 Drv Slave address: 0x14");
            }
            else if (addr == GT911_SLAVE_ADDRESS1)
            {
                touchAddress = GT911_SLAVE_ADDRESS1;
                Serial.println("Find GT911 Drv Slave address: 0x5D");
            }
        }
        else if (err == 4)
        {
            Serial.print("Unknow error at address 0x");
            if (addr < 16)
            {
                Serial.print("0");
            }
            Serial.println(addr, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\r\n");
}
void setup()
{
    // loggingStream = &SoftwareSerial(3, 1);
    // rtc_wdt_protect_off(); 
    // rtc_wdt_disable();
    // disableCore0WDT();
    // disableLoopWDT();
    // esp_task_wdt_delete(NULL);
    Serial.begin(115200); /* prepare for possible serial debug */
    delay(1000);
    Serial.println("Advanced Session Program V1.1: setup()");
    Serial.println("Setting up pins.");
    m = millis();
    pinMode(BOARD_POWERON, OUTPUT);
    digitalWrite(BOARD_POWERON, HIGH);
    //! Set CS on all SPI buses to high level during initialization
    pinMode(BOARD_SDCARD_CS, OUTPUT);
    pinMode(RADIO_CS_PIN, OUTPUT);
    pinMode(BOARD_TFT_CS, OUTPUT);

    digitalWrite(BOARD_SDCARD_CS, HIGH);
    digitalWrite(RADIO_CS_PIN, HIGH);
    digitalWrite(BOARD_TFT_CS, HIGH);

    pinMode(BOARD_SPI_MISO, INPUT_PULLUP);
    SPI.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI); // SD

    pinMode(BOARD_BOOT_PIN, INPUT_PULLUP);
    pinMode(BOARD_TBOX_G02, INPUT_PULLUP);
    pinMode(BOARD_TBOX_G01, INPUT_PULLUP);
    pinMode(BOARD_TBOX_G04, INPUT_PULLUP);
    pinMode(BOARD_TBOX_G03, INPUT_PULLUP);
    pinMode(BOARD_BL_PIN, OUTPUT);
    //Wakeup touch chip
    pinMode(BOARD_TOUCH_INT, OUTPUT);
    digitalWrite(BOARD_TOUCH_INT, HIGH);
    LOGP("Pin setup done in %dms\r\n", millis() - m);
    
    LOG("Initializing the TFT and FS.");
    Wire.begin(BOARD_I2C_SDA, BOARD_I2C_SCL);
    m = millis();
    tft.init();         /* TFT init */
    
    tft.setRotation(1); /* Landscape orientation, flipped */
    tft.setBrightness(255);
    tft.setCursor(0, 0);
    if(!LittleFS.begin()) {
        LittleFS.format();
        Serial.println("LittleFS format done");
        if(!LittleFS.begin()) {
            PANIC("LittleFS.begin() failed after retry.", "check for damages, replace chip or retry by resetting");
        }
    };
    settings::read_from_littlefs();
    // PANIC("Test", "commenting out this line");
    /*Set the touchscreen calibration data,
     the actual data for your display can be acquired using
     the Generic -> Touch_calibrate example from the TFT_eSPI library*/
    // Set touch int input
    
    LOGP("TFT init done in %dms\r\n", millis() - m);
    
    
    // if(esp_task_wdt_deinit() != ESP_OK) {
    //     PANIC("Cannot deinit TWDT.", "fix code or contact author");
    // }
    
    LOG("Initializing LVGL.");
    pqd;
    m = millis();
    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif
    LOGP("LVGL init done in %dms\r\n", millis() - m);
    pqd;
    m = millis();
    pinMode(BOARD_TOUCH_INT, INPUT);
    delay(20);
    scanDevices(&Wire);
    
    touch = new TouchLib(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, touchAddress);

    touch->init();
    LOGP("Touch input done in %dms\r\n", millis() - m);
    LOG("Set brightness.");
    pqd;
    m = millis();
    LOGP("Brightness set in %dms\r\n", millis() - m);
    pqd;

    LOG("LVGL display init.");
    pqd;
    m = millis();
    /*Initialize the display*/
    lv_display_t *disp = lv_display_create(320, 240);
    // lv_display_set_draw_buffers( disp_drv, buf, NULL);
    static lv_color_t * buf = (lv_color_t*)malloc(320*240/10);
    // static lv_color_t * buf2 = (lv_color_t*)heap_caps_malloc(320*240/10, MALLOC_CAP_SPIRAM);
    // static lv_color_t * buf2 = (lv_color_t*)heap_caps_malloc(320*240*sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    lv_display_set_buffers(disp, buf, NULL, 320*240/10, LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_display_set_flush_cb(disp, my_disp_flush);
    
    /*Initialize the (dummy) input device driver*/
    LOGP("LVGL display driver started in %dms\r\n", millis() - m);
    LOG("LVGL input.");
    pqd;
    m = millis();
    lv_indev_t *kbdrv = lv_indev_create();
    lv_indev_set_type(kbdrv, LV_INDEV_TYPE_KEYPAD); /*See below.*/
    lv_indev_set_read_cb(kbdrv, keypad_read);       /*See below.*/
    lv_indev_t *trackdrv = lv_indev_create();
    lv_indev_set_type(trackdrv, LV_INDEV_TYPE_KEYPAD); /*See below.*/
    lv_indev_set_read_cb(trackdrv, trackball_read);    /*See below.*/

    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*See below.*/
    lv_indev_set_read_cb(indev, my_touchpad_read);   /*See below.*/
    // setBrightness(15);
    LOGP("LVGL input driver started in %dms\r\n", millis() - m);
    unsigned long startmillis = millis();
    while(millis() - startmillis < 1000) {
        if(keypad_get_key() == 32) {
            rEnter("Key");
        }
        delay(10);
    }
    LOG("UI initialization.");
    pqd;
    ui_init();
    LOGP("Setup done in %d ms\r\n", millis());
    pqd;
}

/**
 * A function that handles the main loop of the program.
 *
 * @param None
 *
 * @return None
 *
 * @throws None
 */
void loop()
{
    
    unsigned long d = millis();
    // tft.setCursor(0, 0);
    delay(lv_timer_handler());
    lv_tick_inc(millis() - d);
    
}
