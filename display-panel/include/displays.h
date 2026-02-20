#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "config.h"

// GC9A01 display class for LovyanGFX
class LGFX_GC9A01 : public lgfx::LGFX_Device {
    lgfx::Panel_GC9A01 _panel;
    lgfx::Bus_SPI _bus;

public:
    LGFX_GC9A01(int cs_pin) {
        auto cfg_bus = _bus.config();
        cfg_bus.spi_host = SPI2_HOST;
        cfg_bus.spi_mode = 0;
        cfg_bus.freq_write = 40000000;
        cfg_bus.freq_read  = 16000000;
        cfg_bus.pin_mosi = TFT_MOSI;
        cfg_bus.pin_miso = -1;
        cfg_bus.pin_sclk = TFT_SCLK;
        cfg_bus.pin_dc   = TFT_DC;
        _bus.config(cfg_bus);
        _panel.setBus(&_bus);

        auto cfg_panel = _panel.config();
        cfg_panel.pin_cs  = cs_pin;
        cfg_panel.pin_rst = TFT_RST;
        cfg_panel.panel_width  = DISPLAY_WIDTH;
        cfg_panel.panel_height = DISPLAY_HEIGHT;
        cfg_panel.offset_x = 0;
        cfg_panel.offset_y = 0;
        cfg_panel.invert = true;
        _panel.config(cfg_panel);

        setPanel(&_panel);
    }
};

// All 6 displays
extern LGFX_GC9A01* displays[NUM_DISPLAYS];

void initDisplays();
void clearDisplay(int idx, uint32_t color = 0x000000);
