#include "screens.h"
#include "config.h"
#include "gauges.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

// ============================================
// Cached data from API calls
// ============================================

// Unraid
static float  unraidDriveTemps[8] = {0};
static char   unraidDriveNames[8][8] = {"sda","sdb","sdc","sdd","sde","sdf","sdg","sdh"};
static int    unraidDriveCount = 0;
static float  unraidStorageUsedTB = 0;
static float  unraidStorageTotalTB = 0;
static float  unraidCpuPercent = 0;
static float  unraidMemPercent = 0;
static int    unraidDockerRunning = 0;
static int    unraidDockerTotal = 0;
static String unraidArrayStatus = "unknown";

// M900
static float  m900CpuPercent = 0;
static float  m900CpuTemp = 0;
static float  m900MemPercent = 0;
static float  m900MemUsedGB = 0;
static float  m900MemTotalGB = 0;
static float  m900DiskPercent = 0;
static float  m900DiskUsedGB = 0;
static float  m900DiskTotalGB = 0;

// Pi health
static float  piTemps[4] = {0, 0, 0, 0};
static float  piCpu[4] = {0, 0, 0, 0};
static float  piMem[4] = {0, 0, 0, 0};
static bool   piOnline[4] = {false, false, false, false};
static const char* piNames[4] = {"FlightRdr", "Uptime", "Spare-1", "Spare-2"};
static const char* piHosts[4] = {PI_FLIGHT_IP, PI_UPTIME_IP, PI_SPARE1_IP, PI_SPARE2_IP};

// Services
struct ServiceStatus {
    const char* name;
    const char* url;
    bool up;
};

static ServiceStatus services[] = {
    {"Jazz Stats",   JAZZ_STATS_URL,      false},
    {"NHL Tracker",  NHL_TRACKER_URL,     false},
    {"ClaudeCAD",    CLAUDECAD_URL,       false},
    {"Plex",         PLEX_URL,            false},
    {"Sonarr",       SONARR_URL,          false},
    {"Radarr",       RADARR_URL,          false},
    {"Overseerr",    OVERSEERR_URL,       false},
    {"AudioBooks",   AUDIOBOOKSHELF_URL,  false},
    {"Mammoth",      MAMMOTH_URL,         false},
    {"FlightRadar",  FLIGHT_TAR1090_URL,  false},
    {"UptimeKuma",   UPTIME_KUMA_URL,     false},
};
static const int NUM_SERVICES = sizeof(services) / sizeof(services[0]);

// Custom screen - network bandwidth
static long netBytesSent = 0;
static long netBytesRecv = 0;
static long prevBytesSent = 0;
static long prevBytesRecv = 0;
static float netUpMbps = 0;
static float netDownMbps = 0;

// ============================================
// Helper: fetch JSON from URL
// ============================================
static bool fetchJson(const char* url, JsonDocument& doc) {
    HTTPClient http;
    http.begin(url);
    http.setTimeout(3000);
    int code = http.GET();
    bool ok = false;
    if (code == 200) {
        DeserializationError err = deserializeJson(doc, http.getString());
        ok = (err == DeserializationError::Ok);
    }
    http.end();
    return ok;
}

static bool httpCheck(const char* url) {
    HTTPClient http;
    http.begin(url);
    http.setTimeout(2000);
    int code = http.GET();
    http.end();
    return (code > 0 && code < 400);
}

// ============================================
// Screen 0: Unraid Health
// ============================================
void drawUnraid(int idx) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);

    // Title
    d->setTextDatum(middle_center);
    d->setTextSize(1.5);
    d->setTextColor(0xFD20, TFT_BLACK);  // Orange
    d->drawString("UNRAID", 120, 20);

    // Array status indicator
    uint32_t statusColor = (unraidArrayStatus == "STARTED") ? TFT_GREEN : TFT_RED;
    d->fillCircle(120, 38, 4, statusColor);

    // Drive temps as mini bars across the middle
    if (unraidDriveCount > 0) {
        int barWidth = 180 / unraidDriveCount;
        int startX = 120 - (unraidDriveCount * barWidth) / 2;

        d->setTextSize(1);
        d->setTextColor(TFT_LIGHTGREY, TFT_BLACK);
        d->drawString("DRIVE TEMPS", 120, 55);

        for (int i = 0; i < unraidDriveCount && i < 8; i++) {
            int x = startX + i * barWidth + barWidth / 2;
            float temp = unraidDriveTemps[i];

            // Bar height based on temp (20-60°C range)
            int maxH = 50;
            float pct = constrain((temp - 20) / 40.0, 0, 1);
            int barH = (int)(pct * maxH);

            uint32_t color = gaugeColor(temp, 40, 50);
            int barY = 120 - barH / 2;
            d->fillRect(x - barWidth/2 + 2, barY, barWidth - 4, barH, color);

            // Temp label
            char tStr[6];
            snprintf(tStr, sizeof(tStr), "%.0f", temp);
            d->setTextSize(0.8);
            d->setTextColor(TFT_WHITE, TFT_BLACK);
            d->drawString(tStr, x, 120 + maxH/2 + 10);

            // Drive name
            d->setTextColor(TFT_DARKGREY, TFT_BLACK);
            d->drawString(unraidDriveNames[i], x, 120 - maxH/2 - 8);
        }
    }

    // Storage bar at bottom
    d->setTextSize(1);
    d->setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    d->drawString("STORAGE", 120, 165);

    float storagePct = 0;
    if (unraidStorageTotalTB > 0)
        storagePct = (unraidStorageUsedTB / unraidStorageTotalTB) * 100;

    // Draw bar
    int barX = 40, barY = 178, barW = 160, barH = 12;
    d->drawRect(barX, barY, barW, barH, TFT_DARKGREY);
    int fillW = (int)(storagePct / 100.0 * (barW - 2));
    uint32_t barColor = gaugeColor(storagePct, 75, 90);
    d->fillRect(barX + 1, barY + 1, fillW, barH - 2, barColor);

    // Storage text
    char storStr[24];
    snprintf(storStr, sizeof(storStr), "%.1f / %.1fTB", unraidStorageUsedTB, unraidStorageTotalTB);
    d->setTextColor(TFT_WHITE, TFT_BLACK);
    d->drawString(storStr, 120, 200);

    // Docker count
    char dockStr[20];
    snprintf(dockStr, sizeof(dockStr), "%d/%d containers", unraidDockerRunning, unraidDockerTotal);
    d->setTextSize(0.8);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    d->drawString(dockStr, 120, 220);
}

// ============================================
// Screen 1: M900 Health (RPM gauges)
// ============================================
void drawM900(int idx) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);

    // Title
    d->setTextDatum(middle_center);
    d->setTextSize(1.5);
    d->setTextColor(TFT_CYAN, TFT_BLACK);
    d->drawString("M900", 120, 20);

    // CPU gauge (top half, big)
    GaugeConfig cpuGauge = CPU_GAUGE;
    cpuGauge.arcRadius = 55;
    cpuGauge.arcWidth = 10;

    drawArc(d, 120, 85, m900CpuPercent, cpuGauge);

    d->setTextSize(1);
    d->setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    d->drawString("CPU", 120, 60);

    d->setTextSize(2.5);
    uint32_t cpuColor = gaugeColor(m900CpuPercent, 75, 90);
    d->setTextColor(cpuColor, TFT_BLACK);
    char cpuStr[8];
    snprintf(cpuStr, sizeof(cpuStr), "%.0f%%", m900CpuPercent);
    d->drawString(cpuStr, 120, 90);

    // CPU temp below gauge
    d->setTextSize(1);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    char tempStr[10];
    snprintf(tempStr, sizeof(tempStr), "%.0f°C", m900CpuTemp);
    d->drawString(tempStr, 120, 118);

    // RAM and Disk as two mini gauges at bottom
    GaugeConfig miniCfg = SMALL_GAUGE;
    miniCfg.minVal = 0;
    miniCfg.maxVal = 100;
    miniCfg.warnVal = 80;
    miniCfg.critVal = 95;

    // RAM (bottom left)
    char ramStr[8];
    snprintf(ramStr, sizeof(ramStr), "%.0f%%", m900MemPercent);
    drawMiniGauge(d, 72, 185, m900MemPercent, miniCfg, "RAM", ramStr);

    // Disk (bottom right)
    char diskStr[8];
    snprintf(diskStr, sizeof(diskStr), "%.0f%%", m900DiskPercent);
    drawMiniGauge(d, 168, 185, m900DiskPercent, miniCfg, "DISK", diskStr);
}

// ============================================
// Screen 2: Pi Rack Health (4 mini gauges)
// ============================================
void drawPiHealth(int idx) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);

    d->setTextDatum(middle_center);
    d->setTextSize(1.5);
    d->setTextColor(TFT_GREEN, TFT_BLACK);
    d->drawString("PI RACK", 120, 18);

    // 4 mini gauges in a 2x2 grid
    int positions[4][2] = {
        {72,  85},   // top-left
        {168, 85},   // top-right
        {72,  175},  // bottom-left
        {168, 175},  // bottom-right
    };

    GaugeConfig piGauge = SMALL_GAUGE;

    for (int i = 0; i < 4; i++) {
        int cx = positions[i][0];
        int cy = positions[i][1];

        if (piOnline[i]) {
            char valStr[8];
            snprintf(valStr, sizeof(valStr), "%.0f°", piTemps[i]);
            drawMiniGauge(d, cx, cy, piTemps[i], piGauge, piNames[i], valStr);
        } else {
            // Offline indicator
            d->setTextSize(1);
            d->setTextColor(TFT_DARKGREY, TFT_BLACK);
            d->drawString(piNames[i], cx, cy - 12);
            d->setTextSize(1.5);
            d->setTextColor(TFT_RED, TFT_BLACK);
            d->drawString("OFF", cx, cy + 8);
        }
    }
}

// ============================================
// Screen 3: Services Status
// ============================================
void drawServices(int idx) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);

    d->setTextDatum(middle_center);
    d->setTextSize(1.5);
    d->setTextColor(TFT_YELLOW, TFT_BLACK);
    d->drawString("SERVICES", 120, 18);

    // Count up/down
    int upCount = 0;
    for (int i = 0; i < NUM_SERVICES; i++) {
        if (services[i].up) upCount++;
    }

    // Summary
    d->setTextSize(1);
    char sumStr[16];
    snprintf(sumStr, sizeof(sumStr), "%d/%d online", upCount, NUM_SERVICES);
    uint32_t sumColor = (upCount == NUM_SERVICES) ? TFT_GREEN : TFT_YELLOW;
    d->setTextColor(sumColor, TFT_BLACK);
    d->drawString(sumStr, 120, 36);

    // Service list with dots
    int y = 55;
    int spacing = 17;

    for (int i = 0; i < NUM_SERVICES && y < 230; i++) {
        // Status dot
        uint32_t dotColor = services[i].up ? TFT_GREEN : TFT_RED;
        d->fillCircle(50, y, 4, dotColor);

        // Service name
        d->setTextSize(1);
        d->setTextColor(TFT_WHITE, TFT_BLACK);
        d->setTextDatum(middle_left);
        d->drawString(services[i].name, 60, y);
        d->setTextDatum(middle_center);

        y += spacing;
    }
}

// ============================================
// Screen 4: Custom Stats (Network bandwidth)
// ============================================
void drawCustom(int idx) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);

    d->setTextDatum(middle_center);
    d->setTextSize(1.5);
    d->setTextColor(TFT_MAGENTA, TFT_BLACK);
    d->drawString("NETWORK", 120, 20);

    // Download gauge
    GaugeConfig netGauge = DEFAULT_GAUGE;
    netGauge.minVal = 0;
    netGauge.maxVal = 100;  // 100 Mbps scale
    netGauge.warnVal = 50;
    netGauge.critVal = 80;
    netGauge.arcRadius = 55;
    netGauge.arcWidth = 10;

    // Down (top)
    drawArc(d, 120, 88, netDownMbps, netGauge);
    d->setTextSize(1);
    d->setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    d->drawString("DOWN", 120, 63);
    d->setTextSize(2);
    d->setTextColor(TFT_GREEN, TFT_BLACK);
    char downStr[12];
    snprintf(downStr, sizeof(downStr), "%.1f", netDownMbps);
    d->drawString(downStr, 120, 88);
    d->setTextSize(1);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    d->drawString("Mbps", 120, 108);

    // Up (bottom, smaller text)
    d->setTextSize(1);
    d->setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    d->drawString("UP", 120, 150);
    d->setTextSize(2);
    d->setTextColor(TFT_CYAN, TFT_BLACK);
    char upStr[12];
    snprintf(upStr, sizeof(upStr), "%.1f", netUpMbps);
    d->drawString(upStr, 120, 172);
    d->setTextSize(1);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    d->drawString("Mbps", 120, 192);

    // WiFi signal
    d->setTextSize(0.8);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    char wifiStr[20];
    snprintf(wifiStr, sizeof(wifiStr), "WiFi: %ddBm", WiFi.RSSI());
    d->drawString(wifiStr, 120, 220);
}

// ============================================
// Screen 5: Clock
// ============================================
void drawClock(int idx) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);

    // Subtle circle border
    d->drawCircle(120, 120, 118, 0x2104);
    d->drawCircle(120, 120, 119, 0x2104);

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        d->setTextDatum(middle_center);
        d->setTextSize(2);
        d->drawString("No Time", 120, 120);
        return;
    }

    char timeStr[6];
    char ampm[3];
    char dateStr[12];
    char dayStr[10];

    strftime(timeStr, sizeof(timeStr), "%I:%M", &timeinfo);
    strftime(ampm, sizeof(ampm), "%p", &timeinfo);
    strftime(dateStr, sizeof(dateStr), "%b %d", &timeinfo);
    strftime(dayStr, sizeof(dayStr), "%A", &timeinfo);

    // Remove leading zero
    char* t = timeStr;
    if (t[0] == '0') t++;

    d->setTextDatum(middle_center);

    // Time (big)
    d->setTextSize(4);
    d->setTextColor(TFT_WHITE, TFT_BLACK);
    d->drawString(t, 120, 90);

    // AM/PM
    d->setTextSize(1.5);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    d->drawString(ampm, 120, 120);

    // Day
    d->setTextSize(2);
    d->setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    d->drawString(dayStr, 120, 150);

    // Date
    d->setTextSize(1.5);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    d->drawString(dateStr, 120, 175);
}

// ============================================
// Boot splash
// ============================================
void drawBootSplash(int idx, const char* label) {
    auto* d = displays[idx];
    d->fillScreen(TFT_BLACK);
    d->drawCircle(120, 120, 118, 0x2104);
    d->setTextDatum(middle_center);
    d->setTextSize(1.5);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    d->drawString(label, 120, 120);
}

// ============================================
// Data fetch functions
// ============================================

void fetchUnraid() {
    char url[64];
    snprintf(url, sizeof(url), "http://%s:%d/stats", UNRAID_IP, UNRAID_STATS_PORT);

    JsonDocument doc;
    if (fetchJson(url, doc)) {
        // Drives
        JsonArray drives = doc["drives"];
        unraidDriveCount = min((int)drives.size(), 8);
        for (int i = 0; i < unraidDriveCount; i++) {
            unraidDriveTemps[i] = drives[i]["temp_c"] | 0.0f;
            strlcpy(unraidDriveNames[i], drives[i]["device"] | "??", sizeof(unraidDriveNames[i]));
        }

        // Storage
        unraidStorageUsedTB = (doc["storage"]["used_gb"] | 0.0f) / 1024.0;
        unraidStorageTotalTB = (doc["storage"]["total_gb"] | 0.0f) / 1024.0;

        // System
        unraidCpuPercent = doc["system"]["cpu_percent"] | 0.0f;
        unraidMemPercent = doc["system"]["mem_percent"] | 0.0f;

        // Docker
        unraidDockerRunning = doc["docker"]["running"] | 0;
        unraidDockerTotal = doc["docker"]["total"] | 0;

        // Array
        unraidArrayStatus = doc["array_status"].as<String>();
    }
}

void fetchM900() {
    char url[64];
    snprintf(url, sizeof(url), "http://%s:%d/stats", M900_IP, M900_STATS_PORT);

    JsonDocument doc;
    if (fetchJson(url, doc)) {
        m900CpuPercent = doc["cpu"]["percent"] | 0.0f;
        m900CpuTemp = doc["cpu"]["temp_c"] | 0.0f;
        m900MemPercent = doc["memory"]["percent"] | 0.0f;
        m900MemUsedGB = doc["memory"]["used_gb"] | 0.0f;
        m900MemTotalGB = doc["memory"]["total_gb"] | 0.0f;
        m900DiskPercent = doc["disk"]["percent"] | 0.0f;
        m900DiskUsedGB = doc["disk"]["used_gb"] | 0.0f;
        m900DiskTotalGB = doc["disk"]["total_gb"] | 0.0f;

        // Network bandwidth calc
        prevBytesSent = netBytesSent;
        prevBytesRecv = netBytesRecv;
        netBytesSent = doc["network"]["bytes_sent"] | 0L;
        netBytesRecv = doc["network"]["bytes_recv"] | 0L;

        if (prevBytesSent > 0) {
            float intervalSec = M900_UPDATE_MS / 1000.0;
            netUpMbps = ((netBytesSent - prevBytesSent) * 8.0 / 1000000.0) / intervalSec;
            netDownMbps = ((netBytesRecv - prevBytesRecv) * 8.0 / 1000000.0) / intervalSec;
        }
    }
}

void fetchPiHealth() {
    for (int i = 0; i < 4; i++) {
        char url[80];
        snprintf(url, sizeof(url), "http://%s:9200/stats", piHosts[i]);

        JsonDocument doc;
        if (fetchJson(url, doc)) {
            piOnline[i] = true;
            piTemps[i] = doc["cpu"]["temp_c"] | 0.0f;
            piCpu[i] = doc["cpu"]["percent"] | 0.0f;
            piMem[i] = doc["memory"]["percent"] | 0.0f;
        } else {
            piOnline[i] = false;
        }
    }
}

void fetchServices() {
    for (int i = 0; i < NUM_SERVICES; i++) {
        services[i].up = httpCheck(services[i].url);
    }
}

void fetchCustom() {
    // Network stats are pulled from M900 fetch
    // Nothing extra needed here
}
