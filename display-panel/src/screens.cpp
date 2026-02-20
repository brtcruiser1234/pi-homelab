#include "screens.h"
#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

// ============================================
// Cached data from API calls
// ============================================

// Weather
static String weatherDesc = "--";
static float  weatherTemp = 0;
static float  weatherHigh = 0;
static float  weatherLow  = 0;
static int    weatherHumidity = 0;
static String weatherIcon = "";

// Flights
static int flightCount = 0;
static int flightMessages = 0;

// Services (true = up)
static bool svcJazz     = false;
static bool svcNHL      = false;
static bool svcPlex     = false;
static bool svcPihole   = false;
static bool svcRadar    = false;
static bool svcUptime   = false;

// Sports
static String jazzScore    = "--";
static String jazzOpponent = "--";
static String mammothScore = "--";

// ============================================
// Helper: draw centered text in a circular area
// ============================================

static void drawCircleFrame(LGFX_GC9A01* d, uint32_t borderColor) {
    d->drawCircle(120, 120, 118, borderColor);
    d->drawCircle(120, 120, 119, borderColor);
}

// ============================================
// Screen 0: Clock + Date
// ============================================

void drawClock(int idx) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);
    drawCircleFrame(d, TFT_DARKGREY);

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        d->setTextSize(2);
        d->drawString("No Time", 120, 120);
        return;
    }

    char timeStr[6];
    char dateStr[12];
    char dayStr[10];

    strftime(timeStr, sizeof(timeStr), "%I:%M", &timeinfo);
    strftime(dateStr, sizeof(dateStr), "%b %d", &timeinfo);
    strftime(dayStr, sizeof(dayStr), "%A", &timeinfo);

    // Remove leading zero from hour
    char* t = timeStr;
    if (t[0] == '0') t++;

    d->setTextSize(4);
    d->setTextColor(TFT_WHITE, TFT_BLACK);
    d->drawString(t, 120, 100);

    d->setTextSize(2);
    d->setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    d->drawString(dayStr, 120, 145);

    d->setTextSize(1.5);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    d->drawString(dateStr, 120, 170);
}

// ============================================
// Screen 1: Weather
// ============================================

void drawWeather(int idx) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);
    drawCircleFrame(d, TFT_CYAN);

    d->setTextSize(1.5);
    d->setTextColor(TFT_CYAN, TFT_BLACK);
    d->drawString("WEATHER", 120, 40);

    // Current temp (big)
    d->setTextSize(4);
    d->setTextColor(TFT_WHITE, TFT_BLACK);
    char tempStr[8];
    snprintf(tempStr, sizeof(tempStr), "%.0f°", weatherTemp);
    d->drawString(tempStr, 120, 95);

    // Description
    d->setTextSize(1.5);
    d->setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    d->drawString(weatherDesc.c_str(), 120, 135);

    // High/Low
    d->setTextSize(1.5);
    d->setTextColor(TFT_RED, TFT_BLACK);
    char hiStr[10];
    snprintf(hiStr, sizeof(hiStr), "H:%.0f°", weatherHigh);
    d->drawString(hiStr, 80, 170);

    d->setTextColor(TFT_BLUE, TFT_BLACK);
    char loStr[10];
    snprintf(loStr, sizeof(loStr), "L:%.0f°", weatherLow);
    d->drawString(loStr, 160, 170);

    // Humidity
    d->setTextSize(1);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    char humStr[12];
    snprintf(humStr, sizeof(humStr), "%d%% humid", weatherHumidity);
    d->drawString(humStr, 120, 200);
}

// ============================================
// Screen 2: Flight Radar
// ============================================

void drawFlights(int idx) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);
    drawCircleFrame(d, TFT_GREEN);

    d->setTextSize(1.5);
    d->setTextColor(TFT_GREEN, TFT_BLACK);
    d->drawString("FLIGHTS", 120, 40);

    // Aircraft icon (simple text for now)
    d->setTextSize(2);
    d->setTextColor(TFT_WHITE, TFT_BLACK);
    d->drawString("~==>", 120, 80);

    // Count (big)
    d->setTextSize(5);
    d->setTextColor(TFT_WHITE, TFT_BLACK);
    char countStr[6];
    snprintf(countStr, sizeof(countStr), "%d", flightCount);
    d->drawString(countStr, 120, 130);

    d->setTextSize(1.5);
    d->setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    d->drawString("aircraft", 120, 170);

    // Messages
    d->setTextSize(1);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    char msgStr[20];
    snprintf(msgStr, sizeof(msgStr), "%d msg/s", flightMessages);
    d->drawString(msgStr, 120, 200);
}

// ============================================
// Screen 3: Service Status
// ============================================

static void drawStatusDot(LGFX_GC9A01* d, int x, int y, bool up, const char* label) {
    d->fillCircle(x - 40, y, 6, up ? TFT_GREEN : TFT_RED);
    d->setTextSize(1.5);
    d->setTextColor(TFT_WHITE, TFT_BLACK);
    d->setTextDatum(middle_left);
    d->drawString(label, x - 28, y);
    d->setTextDatum(middle_center);
}

void drawServices(int idx) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);
    drawCircleFrame(d, TFT_YELLOW);

    d->setTextSize(1.5);
    d->setTextColor(TFT_YELLOW, TFT_BLACK);
    d->drawString("SERVICES", 120, 35);

    int y = 65;
    int spacing = 28;

    drawStatusDot(d, 120, y,              svcJazz,   "Jazz Stats");
    drawStatusDot(d, 120, y + spacing,    svcNHL,    "NHL Track");
    drawStatusDot(d, 120, y + spacing*2,  svcPlex,   "Plex");
    drawStatusDot(d, 120, y + spacing*3,  svcPihole, "Flight Rdr");
    drawStatusDot(d, 120, y + spacing*4,  svcRadar,  "Uptime");
    drawStatusDot(d, 120, y + spacing*5,  svcUptime, "Unraid");
}

// ============================================
// Screen 4: System Stats
// ============================================

void drawStats(int idx) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);
    drawCircleFrame(d, TFT_MAGENTA);

    d->setTextSize(1.5);
    d->setTextColor(TFT_MAGENTA, TFT_BLACK);
    d->drawString("SYSTEM", 120, 40);

    // ESP32 stats
    d->setTextSize(1.5);
    d->setTextColor(TFT_WHITE, TFT_BLACK);

    char wifiStr[20];
    snprintf(wifiStr, sizeof(wifiStr), "WiFi: %ddBm", WiFi.RSSI());
    d->drawString(wifiStr, 120, 80);

    char heapStr[20];
    snprintf(heapStr, sizeof(heapStr), "Heap: %dKB", ESP.getFreeHeap() / 1024);
    d->drawString(heapStr, 120, 110);

    char uptimeStr[20];
    unsigned long secs = millis() / 1000;
    unsigned long hrs = secs / 3600;
    unsigned long mins = (secs % 3600) / 60;
    snprintf(uptimeStr, sizeof(uptimeStr), "Up: %luh %lum", hrs, mins);
    d->drawString(uptimeStr, 120, 140);

    // IP address
    d->setTextSize(1);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    d->drawString(WiFi.localIP().toString().c_str(), 120, 180);

    d->drawString(WiFi.SSID().c_str(), 120, 200);
}

// ============================================
// Screen 5: Sports Scores
// ============================================

void drawSports(int idx) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);
    drawCircleFrame(d, TFT_ORANGE);

    d->setTextSize(1.5);
    d->setTextColor(TFT_ORANGE, TFT_BLACK);
    d->drawString("SPORTS", 120, 35);

    // Jazz
    d->setTextSize(1.5);
    d->setTextColor(0x2E64FE, TFT_BLACK);  // Jazz blue
    d->drawString("JAZZ", 120, 70);
    d->setTextSize(2);
    d->setTextColor(TFT_WHITE, TFT_BLACK);
    d->drawString(jazzScore.c_str(), 120, 100);
    d->setTextSize(1);
    d->setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    d->drawString(jazzOpponent.c_str(), 120, 125);

    // Divider
    d->drawLine(60, 145, 180, 145, TFT_DARKGREY);

    // Mammoth
    d->setTextSize(1.5);
    d->setTextColor(0x1B3C6B, TFT_BLACK);  // Mammoth navy
    d->drawString("MAMMOTH", 120, 165);
    d->setTextSize(2);
    d->setTextColor(TFT_WHITE, TFT_BLACK);
    d->drawString(mammothScore.c_str(), 120, 195);
}

// ============================================
// Boot splash
// ============================================

void drawBootSplash(int idx, const char* label) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);
    drawCircleFrame(d, TFT_DARKGREY);
    d->setTextSize(1.5);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    d->drawString(label, 120, 120);
}

// ============================================
// Data fetch functions
// ============================================

void fetchWeather() {
    if (strlen(OWM_API_KEY) < 10) return;  // No API key set

    HTTPClient http;
    String url = "http://api.openweathermap.org/data/2.5/weather?lat=";
    url += OWM_LAT;
    url += "&lon=";
    url += OWM_LON;
    url += "&units=";
    url += OWM_UNITS;
    url += "&appid=";
    url += OWM_API_KEY;

    http.begin(url);
    int code = http.GET();
    if (code == 200) {
        JsonDocument doc;
        deserializeJson(doc, http.getString());
        weatherTemp = doc["main"]["temp"];
        weatherHigh = doc["main"]["temp_max"];
        weatherLow  = doc["main"]["temp_min"];
        weatherHumidity = doc["main"]["humidity"];
        weatherDesc = doc["weather"][0]["main"].as<String>();
    }
    http.end();
}

void fetchFlights() {
    HTTPClient http;
    String url = "http://";
    url += FLIGHT_RADAR_IP;
    url += "/tar1090/data/stats.json";

    http.begin(url);
    http.setTimeout(3000);
    int code = http.GET();
    if (code == 200) {
        JsonDocument doc;
        deserializeJson(doc, http.getString());
        flightCount = doc["last1min"]["tracks"]["all"] | 0;
        flightMessages = doc["last1min"]["messages"] | 0;
    }
    http.end();
}

void fetchServices() {
    auto check = [](const char* host, int port) -> bool {
        HTTPClient http;
        String url = "http://";
        url += host;
        url += ":";
        url += port;
        http.begin(url);
        http.setTimeout(2000);
        int code = http.GET();
        http.end();
        return (code > 0 && code < 400);
    };

    svcJazz   = check(M900_IP, JAZZ_STATS_PORT);
    svcNHL    = check(M900_IP, NHL_TRACKER_PORT);
    svcPlex   = check(UNRAID_IP, 32400);
    svcPihole = check(FLIGHT_RADAR_IP, 80);
    svcRadar  = check(UPTIME_KUMA_IP, UPTIME_KUMA_PORT);

    // Ping Unraid
    HTTPClient http;
    String url = "http://";
    url += UNRAID_IP;
    http.begin(url);
    http.setTimeout(2000);
    int code = http.GET();
    http.end();
    svcUptime = (code > 0);
}

void fetchStats() {
    // ESP32 internal stats are read directly in drawStats()
    // Could add API calls to Pi endpoints here for their CPU temps
}

void fetchSports() {
    // Pull from Jazz-Stats API
    HTTPClient http;
    String url = "http://";
    url += M900_IP;
    url += ":";
    url += JAZZ_STATS_PORT;

    http.begin(url);
    http.setTimeout(3000);
    int code = http.GET();
    if (code == 200) {
        // Jazz-Stats returns HTML, would need a JSON endpoint
        // For now, show placeholder
        jazzScore = "-- - --";
        jazzOpponent = "";
    }
    http.end();

    // Mammoth stats
    url = "http://";
    url += UNRAID_IP;
    url += ":";
    url += MAMMOTH_STATS_PORT;

    http.begin(url);
    http.setTimeout(3000);
    code = http.GET();
    if (code == 200) {
        mammothScore = "-- - --";
    }
    http.end();
}
