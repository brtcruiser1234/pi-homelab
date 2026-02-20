#pragma once

// ============================================
// WiFi - configured via WiFiManager captive portal
// on first boot, connect to "RackDisplay" AP
// ============================================

// ============================================
// Display Hardware - 6x GC9A01 1.28" Round TFT
// All share SPI bus, individual CS pins
// ============================================

// Shared SPI pins (ESP32-S3)
#define TFT_MOSI   11
#define TFT_SCLK   12
#define TFT_DC     13
#define TFT_RST    14

// Individual chip select per display
#define TFT_CS_1   10   // Display 1 (leftmost)
#define TFT_CS_2   9    // Display 2
#define TFT_CS_3   46   // Display 3
#define TFT_CS_4   3    // Display 4
#define TFT_CS_5   8    // Display 5
#define TFT_CS_6   18   // Display 6 (rightmost)

// Display properties
#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  240
#define NUM_DISPLAYS    6

// ============================================
// Data Sources - your homelab services
// Update IPs after everything is on the network
// ============================================

// M900 (main server)
#define M900_IP             "10.1.10.XXX"
#define JAZZ_STATS_PORT     8888
#define NHL_TRACKER_PORT    3050

// Unraid
#define UNRAID_IP           "10.1.10.193"
#define MAMMOTH_STATS_PORT  9999

// Pis (update after setup)
#define FLIGHT_RADAR_IP     "flight-radar.local"
#define UPTIME_KUMA_IP      "uptime-kuma.local"
#define UPTIME_KUMA_PORT    3001

// ============================================
// Update intervals (milliseconds)
// ============================================
#define CLOCK_UPDATE_MS     1000      // 1 second
#define WEATHER_UPDATE_MS   600000    // 10 minutes
#define STATS_UPDATE_MS     30000     // 30 seconds
#define FLIGHT_UPDATE_MS    15000     // 15 seconds
#define SERVICES_UPDATE_MS  60000     // 1 minute

// ============================================
// Weather (OpenWeatherMap - free tier)
// Sign up at: https://openweathermap.org/api
// ============================================
#define OWM_API_KEY         "YOUR_API_KEY_HERE"
#define OWM_LAT             "YOUR_LATITUDE"
#define OWM_LON             "YOUR_LONGITUDE"
#define OWM_UNITS           "imperial"

// ============================================
// Display assignments (which screen shows what)
// 0-5 from left to right
// ============================================
#define SCREEN_CLOCK        0
#define SCREEN_WEATHER      1
#define SCREEN_FLIGHTS      2
#define SCREEN_SERVICES     3
#define SCREEN_STATS        4
#define SCREEN_SPORTS       5
