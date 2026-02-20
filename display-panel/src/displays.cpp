#include "displays.h"

static const int cs_pins[NUM_DISPLAYS] = {
    TFT_CS_1, TFT_CS_2, TFT_CS_3,
    TFT_CS_4, TFT_CS_5, TFT_CS_6
};

LGFX_GC9A01* displays[NUM_DISPLAYS];

void initDisplays() {
    for (int i = 0; i < NUM_DISPLAYS; i++) {
        displays[i] = new LGFX_GC9A01(cs_pins[i]);
        displays[i]->init();
        displays[i]->setRotation(0);
        displays[i]->setBrightness(200);
        displays[i]->fillScreen(TFT_BLACK);
        displays[i]->setTextColor(TFT_WHITE, TFT_BLACK);
        displays[i]->setTextDatum(middle_center);
    }
}

void clearDisplay(int idx, uint32_t color) {
    if (idx >= 0 && idx < NUM_DISPLAYS) {
        displays[idx]->fillScreen(color);
    }
}
