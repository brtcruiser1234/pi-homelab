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
// Each machine runs a small stats API script
// Update IPs after everything is on the network
// ============================================

// M900 (main server) - runs stats-api.py on port 9200
#define M900_IP             "10.1.10.XXX"
#define M900_STATS_PORT     9200

// Unraid - runs stats-api.sh via user scripts on port 9201
#define UNRAID_IP           "10.1.10.193"
#define UNRAID_STATS_PORT   9201

// Pis - each runs stats-api.py on port 9200
#define PI_FLIGHT_IP        "flight-radar.local"
#define PI_UPTIME_IP        "uptime-kuma.local"
#define PI_SPARE1_IP        "pi-spare.local"
#define PI_SPARE2_IP        "pi-spare.local"

// Service endpoints for health checks
#define JAZZ_STATS_URL      "http://10.1.10.XXX:8888"
#define NHL_TRACKER_URL     "http://10.1.10.XXX:3050"
#define CLAUDECAD_URL       "http://10.1.10.XXX:4000"
#define PLEX_URL            "http://10.1.10.193:32400"
#define SONARR_URL          "http://10.1.10.193:8989"
#define RADARR_URL          "http://10.1.10.193:7878"
#define OVERSEERR_URL       "http://10.1.10.193:5055"
#define AUDIOBOOKSHELF_URL  "http://10.1.10.193:13378"
#define MAMMOTH_URL         "http://10.1.10.193:9999"
#define FLIGHT_TAR1090_URL  "http://flight-radar.local/tar1090"
#define UPTIME_KUMA_URL     "http://uptime-kuma.local:3001"

// ============================================
// Update intervals (milliseconds)
// ============================================
#define CLOCK_UPDATE_MS     1000      // 1 second
#define UNRAID_UPDATE_MS    15000     // 15 seconds
#define M900_UPDATE_MS      10000     // 10 seconds
#define PI_UPDATE_MS        15000     // 15 seconds
#define SERVICES_UPDATE_MS  30000     // 30 seconds
#define CUSTOM_UPDATE_MS    10000     // 10 seconds

// ============================================
// Screen assignments (which screen shows what)
// 0-5 from left to right
// ============================================
#define SCREEN_UNRAID       0   // Unraid drive temps + storage
#define SCREEN_M900         1   // M900 CPU/RAM gauges
#define SCREEN_PIHEALTH     2   // All 4 Pi temps
#define SCREEN_SERVICES     3   // Service up/down status
#define SCREEN_CUSTOM       4   // Custom stats gauge
#define SCREEN_CLOCK        5   // Clock (lowest priority, rightmost)
