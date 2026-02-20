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
static unsigned long lastWeather  = 0;
static unsigned long lastFlights  = 0;
static unsigned long lastServices = 0;
static unsigned long lastStats    = 0;
static unsigned long lastSports   = 0;

// ============================================
// WiFi setup via captive portal
// ============================================
void setupWiFi() {
    WiFiManager wm;

    // Auto-connect or launch "RackDisplay" AP for config
    wm.setConfigPortalTimeout(180);  // 3 min timeout
    wm.setConnectTimeout(30);

    if (!wm.autoConnect("RackDisplay", "rackdisplay")) {
        Serial.println("WiFi failed, restarting...");
        ESP.restart();
    }

    Serial.print("Connected to WiFi. IP: ");
    Serial.println(WiFi.localIP());
}

// ============================================
// NTP time sync
// ============================================
void setupTime() {
    configTime(-7 * 3600, 3600, "pool.ntp.org", "time.nist.gov");

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
    Serial.println("\n=== Rack Display Panel ===");

    // Init all 6 displays
    initDisplays();
    Serial.println("Displays initialized");

    // Show boot splash on each screen
    const char* labels[] = {
        "CLOCK", "WEATHER", "FLIGHTS",
        "SERVICES", "STATS", "SPORTS"
    };
    for (int i = 0; i < NUM_DISPLAYS; i++) {
        drawBootSplash(i, labels[i]);
    }

    // Connect to WiFi
    Serial.println("Starting WiFi...");
    drawBootSplash(SCREEN_CLOCK, "WiFi...");
    setupWiFi();
    drawBootSplash(SCREEN_CLOCK, "WiFi OK");

    // Sync time
    Serial.println("Syncing time...");
    drawBootSplash(SCREEN_CLOCK, "NTP...");
    setupTime();

    // Initial data fetch
    Serial.println("Fetching initial data...");
    fetchWeather();
    fetchFlights();
    fetchServices();
    fetchStats();
    fetchSports();

    Serial.println("Setup complete. Running...");
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

    // Weather - every 10 minutes
    if (now - lastWeather >= WEATHER_UPDATE_MS) {
        lastWeather = now;
        fetchWeather();
        drawWeather(SCREEN_WEATHER);
    }

    // Flights - every 15 seconds
    if (now - lastFlights >= FLIGHT_UPDATE_MS) {
        lastFlights = now;
        fetchFlights();
        drawFlights(SCREEN_FLIGHTS);
    }

    // Services - every 60 seconds
    if (now - lastServices >= SERVICES_UPDATE_MS) {
        lastServices = now;
        fetchServices();
        drawServices(SCREEN_SERVICES);
    }

    // Stats - every 30 seconds
    if (now - lastStats >= STATS_UPDATE_MS) {
        lastStats = now;
        fetchStats();
        drawStats(SCREEN_STATS);
    }

    // Sports - every 30 seconds
    if (now - lastSports >= STATS_UPDATE_MS) {
        lastSports = now;
        fetchSports();
        drawSports(SCREEN_SPORTS);
    }

    // Small delay to prevent tight loop
    delay(10);
}
