# Rack Display Panel - ESP32-S3 + 6x Round TFTs

1U rack-mounted status panel with 6x 1.28" round GC9A01 TFTs driven by an ESP32-S3-DevKitC. Shows real-time health of your entire homelab with RPM-style gauges.

## Screens (left to right)

| # | Screen | What | Update |
|---|--------|------|--------|
| 1 | **UNRAID** | Drive temps (bar chart), storage usage, array status, docker count | 15s |
| 2 | **M900** | CPU gauge + temp, RAM gauge, Disk gauge | 10s |
| 3 | **PI RACK** | 4 mini temp gauges (one per Pi, shows offline status) | 15s |
| 4 | **SERVICES** | Green/red dots for all 11 services across M900 + Unraid + Pis | 30s |
| 5 | **NETWORK** | Download/upload bandwidth gauge (Mbps) | 10s |
| 6 | **CLOCK** | Time, AM/PM, day, date | 1s |

## Hardware

- **MCU:** ESP32-S3-DevKitC (flashing over Tasmota)
- **Displays:** 6x GC9A01 1.28" round TFT (240x240, SPI)
- **Mount:** Custom 3D printed 1U faceplate for Lab Rax 10" rack

## Data Architecture

```
                    ┌─────────────┐
                    │  ESP32-S3   │ ← pulls JSON every 10-30s
                    │  6x TFTs   │
                    └──────┬──────┘
                           │ HTTP/JSON (WiFi)
          ┌────────────────┼────────────────┐
          ▼                ▼                ▼
   ┌─────────────┐  ┌───────────┐  ┌──────────────┐
   │ M900 :9200  │  │Unraid:9201│  │ Pis :9200    │
   │ m900-stats  │  │unraid-stats│ │ pi-stats     │
   └─────────────┘  └───────────┘  └──────────────┘
          │                │                │
          └────────────────┼────────────────┘
                           │ same JSON APIs
                    ┌──────┴──────┐
                    │ Web Dashboard│ ← browser pulls same data
                    │ M900 :9300  │
                    └─────────────┘
```

## Wiring

| Signal | GPIO | Notes |
|--------|------|-------|
| MOSI | 11 | Shared |
| SCLK | 12 | Shared |
| DC | 13 | Shared |
| RST | 14 | Shared |
| CS 1 | 10 | Unraid |
| CS 2 | 9 | M900 |
| CS 3 | 46 | Pi Rack |
| CS 4 | 3 | Services |
| CS 5 | 8 | Network |
| CS 6 | 18 | Clock |
| VCC | 3.3V | All displays |
| GND | GND | All displays |

## Building & Flashing

```bash
# Install PlatformIO CLI
pip install platformio

# Build and flash (hold BOOT on S3, press RESET, release BOOT)
cd display-panel
pio run -t upload

# Monitor serial output
pio device monitor
```

## First Boot

1. ESP32 creates WiFi AP: **RackDisplay** (password: `rackdisplay`)
2. Connect phone → captive portal opens → select home WiFi
3. Displays boot up and start pulling data

## Configuration

Edit `include/config.h` with your actual IPs before flashing.

## Dependencies

- **LovyanGFX** - Fast GC9A01 display driver
- **ArduinoJson** - JSON parsing
- **WiFiManager** - Captive portal WiFi config
