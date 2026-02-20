# Rack Display Panel - ESP32-S3 + 6x Round TFTs

1U rack-mounted display panel with 6x 1.28" round GC9A01 TFTs driven by an ESP32-S3-DevKitC.

## Screens (left to right)

| # | Screen | Data Source | Update Rate |
|---|--------|-------------|-------------|
| 1 | Clock + Date | NTP | 1s |
| 2 | Weather | OpenWeatherMap API | 10min |
| 3 | Flight Radar | flight-radar Pi (tar1090) | 15s |
| 4 | Service Status | HTTP health checks | 60s |
| 5 | System Stats | ESP32 internal + WiFi | 30s |
| 6 | Sports Scores | Jazz-Stats + Mammoth Stats | 30s |

## Hardware

- **MCU:** ESP32-S3-DevKitC
- **Displays:** 6x GC9A01 1.28" round TFT (240x240, SPI)
- **Mount:** Custom 3D printed 1U faceplate for Lab Rax 10" rack

## Wiring

All displays share one SPI bus with individual chip selects:

| Signal | GPIO | Notes |
|--------|------|-------|
| MOSI | 11 | Shared - data to all displays |
| SCLK | 12 | Shared - clock |
| DC | 13 | Shared - data/command |
| RST | 14 | Shared - reset |
| CS 1 | 10 | Display 1 (clock) |
| CS 2 | 9 | Display 2 (weather) |
| CS 3 | 46 | Display 3 (flights) |
| CS 4 | 3 | Display 4 (services) |
| CS 5 | 8 | Display 5 (stats) |
| CS 6 | 18 | Display 6 (sports) |
| VCC | 3.3V | All displays |
| GND | GND | All displays |

## Building & Flashing

### Prerequisites
- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)
- USB-C cable

### Flash over Tasmota
1. Put ESP32-S3 in download mode: hold BOOT button, press RESET, release BOOT
2. Connect USB-C to computer
3. Build and flash:

```bash
cd display-panel
pio run -t upload
```

### First Boot WiFi Setup
1. ESP32 creates a WiFi AP called **"RackDisplay"**
2. Connect to it from your phone (password: `rackdisplay`)
3. Browser opens captive portal - select your home WiFi and enter password
4. ESP32 saves credentials and connects automatically on future boots

## Configuration

Edit `include/config.h`:
1. Update IP addresses for your M900 and Pis
2. Add your OpenWeatherMap API key (free at https://openweathermap.org/api)
3. Set your latitude/longitude for weather
4. Rearrange screen assignments if desired

## Dependencies

- **LovyanGFX** - Fast display driver (supports GC9A01 natively)
- **ArduinoJson** - JSON parsing for API responses
- **WiFiManager** - Captive portal for WiFi config (no hardcoded passwords)

## TODO

- [ ] Add JSON API endpoints to Jazz-Stats and Mammoth Stats for easy parsing
- [ ] Add screen brightness auto-dim (time-based or ambient sensor)
- [ ] Add OTA updates so you don't need USB to reflash
- [ ] Custom graphics/icons for weather conditions
- [ ] Screen rotation/carousel mode
