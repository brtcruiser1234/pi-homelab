#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <time.h>
#include "config.h"
#include "displays.h"
#include "screens.h"

// ============================================
// Timing
// ============================================
static unsigned long lastClock    = 0;
static unsigned long lastUnraid   = 0;
static unsigned long lastM900     = 0;
static unsigned long lastPi       = 0;
static unsigned long lastServices = 0;
static unsigned long lastCustom   = 0;

// ============================================
// WiFi setup via captive portal
// ============================================
void setupWiFi() {
    WiFiManager wm;
    wm.setConfigPortalTimeout(180);
    wm.setConnectTimeout(30);

    if (!wm.autoConnect("RackDisplay", "rackdisplay")) {
        Serial.println("WiFi failed, restarting...");
        ESP.restart();
    }

    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());
}

// ============================================
// NTP time sync (Mountain Time)
// ============================================
void setupTime() {
    configTzTime("MST7MDT,M3.2.0,M11.1.0", "pool.ntp.org", "time.nist.gov");

    Serial.print("Syncing time");
    struct tm timeinfo;
    int attempts = 0;
    while (!getLocalTime(&timeinfo) && attempts < 10) {
        Serial.print(".");
        delay(500);
        attempts++;
    }
    Serial.println(" done");
}

// ============================================
// Setup
// ============================================
void setup() {
    Serial.begin(115200);
    Serial.println("\n=== Rack Status Panel ===");

    // Init all 6 displays
    initDisplays();
    Serial.println("Displays initialized");

    // Boot splash
    const char* labels[] = {
        "UNRAID", "M900", "PI RACK",
        "SERVICES", "NETWORK", "CLOCK"
    };
    for (int i = 0; i < NUM_DISPLAYS; i++) {
        drawBootSplash(i, labels[i]);
    }

    // WiFi
    drawBootSplash(SCREEN_CLOCK, "WiFi...");
    setupWiFi();
    drawBootSplash(SCREEN_CLOCK, "WiFi OK");

    // Time
    drawBootSplash(SCREEN_CLOCK, "NTP...");
    setupTime();

    // Initial data fetch
    Serial.println("Fetching initial data...");
    fetchUnraid();
    fetchM900();
    fetchPiHealth();
    fetchServices();
    fetchCustom();

    // Draw all screens once
    drawUnraid(SCREEN_UNRAID);
    drawM900(SCREEN_M900);
    drawPiHealth(SCREEN_PIHEALTH);
    drawServices(SCREEN_SERVICES);
    drawCustom(SCREEN_CUSTOM);
    drawClock(SCREEN_CLOCK);

    Serial.println("Running.");
}

// ============================================
// Main loop
// ============================================
void loop() {
    unsigned long now = millis();

    // Clock - every second
    if (now - lastClock >= CLOCK_UPDATE_MS) {
        lastClock = now;
        drawClock(SCREEN_CLOCK);
    }

    // Unraid - every 15 seconds
    if (now - lastUnraid >= UNRAID_UPDATE_MS) {
        lastUnraid = now;
        fetchUnraid();
        drawUnraid(SCREEN_UNRAID);
    }

    // M900 - every 10 seconds
    if (now - lastM900 >= M900_UPDATE_MS) {
        lastM900 = now;
        fetchM900();
        drawM900(SCREEN_M900);
    }

    // Pi health - every 15 seconds
    if (now - lastPi >= PI_UPDATE_MS) {
        lastPi = now;
        fetchPiHealth();
        drawPiHealth(SCREEN_PIHEALTH);
    }

    // Services - every 30 seconds
    if (now - lastServices >= SERVICES_UPDATE_MS) {
        lastServices = now;
        fetchServices();
        drawServices(SCREEN_SERVICES);
    }

    // Custom (network) - every 10 seconds
    if (now - lastCustom >= CUSTOM_UPDATE_MS) {
        lastCustom = now;
        fetchCustom();
        drawCustom(SCREEN_CUSTOM);
    }

    delay(10);
}
