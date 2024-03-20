#include <settings.hpp>

namespace settings {
    void read_from_littlefs() {
        if (LittleFS.exists("/settings.bin")) {
            File f = LittleFS.open("/settings.bin", "r");
            if (f) {
                f.read((uint8_t*)&uSettings, sizeof(uSettings));
                f.close();
            }
        }
    }

    void write_to_littlefs() {
        File f = LittleFS.open("/settings.bin", "w");
        if (f) {
            f.write((uint8_t*)&uSettings, sizeof(uSettings));
            f.close();
        }
    }
}
