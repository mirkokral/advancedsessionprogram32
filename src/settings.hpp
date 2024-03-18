#pragma once

#include <LittleFS.h>

enum {
    TRACKBALL_MODE_NONE=0,
    TRACKBALL_MODE_ARROW=1,
    TRACKBALL_MODE_CURSOR=2,
    TRACKBALL_MODE_NAVIGATE=3
};



struct {
    int trackball_mode = TRACKBALL_MODE_ARROW;
} uSettings;


namespace settings {
    void read_from_littlefs();
    void write_to_littlefs();
}