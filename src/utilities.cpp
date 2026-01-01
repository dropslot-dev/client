#include "utilities.h"
#include "config.h"

esp_chip_info_t chip_info;

void setChipInfo() {
    // Get ESP32 chip information
    esp_chip_info(&chip_info);
}

// Function to convert chip model enum to string
const char* getChipModelString(esp_chip_model_t model) {
    switch (model) {
        case CHIP_ESP32:
            return "ESP32";
        case CHIP_ESP32S2:
            return "ESP32-S2";
        case CHIP_ESP32S3:
            return "ESP32-S3";
        case CHIP_ESP32C3:
            return "ESP32-C3";
        default:
            return "Unknown";
    }
}

String getTimestamp() {
    struct tm timeinfo;
    static char lastBuffer[12] = "[--:--:--]";
    static bool hasValidTime = false;

    // IMPORTANT: default getLocalTime() can block for ~5s if time isn't synced yet.
    // Use a 0ms timeout so logging never stalls.
    if (getLocalTime(&timeinfo, 0)) {
        strftime(lastBuffer, sizeof(lastBuffer), "[%H:%M:%S]", &timeinfo);
        hasValidTime = true;
    }

    if (hasValidTime) {
        return String(lastBuffer);
    }
    return "[--:--:--]";
}
