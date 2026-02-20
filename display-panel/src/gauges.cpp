#include "gauges.h"
#include <math.h>

#ifndef DEG2RAD
#define DEG2RAD 0.017453292f
#endif

// ============================================
// Color based on thresholds
// ============================================
uint32_t gaugeColor(float value, float warn, float crit) {
    if (value >= crit) return TFT_RED;
    if (value >= warn) return TFT_YELLOW;
    return TFT_GREEN;
}

// ============================================
// Draw arc background + filled portion
// ============================================
void drawArc(LGFX_GC9A01* d, int cx, int cy,
             float value, const GaugeConfig& cfg) {

    int r_outer = cfg.arcRadius;
    int r_inner = cfg.arcRadius - cfg.arcWidth;

    // Clamp value to range
    float v = constrain(value, cfg.minVal, cfg.maxVal);
    float pct = (v - cfg.minVal) / (cfg.maxVal - cfg.minVal);

    // Draw background arc (dark grey)
    for (int a = 0; a <= cfg.sweepAngle; a++) {
        float rad = (cfg.startAngle + a) * DEG2RAD;
        float cs = cos(rad);
        float sn = sin(rad);

        for (int r = r_inner; r <= r_outer; r++) {
            int px = cx + (int)(r * cs);
            int py = cy + (int)(r * sn);
            d->drawPixel(px, py, 0x2104);  // dark grey
        }
    }

    // Draw filled arc with color gradient
    int fillAngle = (int)(pct * cfg.sweepAngle);
    for (int a = 0; a <= fillAngle; a++) {
        float segPct = (float)a / cfg.sweepAngle;
        float segVal = cfg.minVal + segPct * (cfg.maxVal - cfg.minVal);
        uint32_t color = gaugeColor(segVal, cfg.warnVal, cfg.critVal);

        float rad = (cfg.startAngle + a) * DEG2RAD;
        float cs = cos(rad);
        float sn = sin(rad);

        for (int r = r_inner; r <= r_outer; r++) {
            int px = cx + (int)(r * cs);
            int py = cy + (int)(r * sn);
            d->drawPixel(px, py, color);
        }
    }

    // Draw needle line
    float needleRad = (cfg.startAngle + fillAngle) * DEG2RAD;
    int nx1 = cx + (int)((r_inner - 4) * cos(needleRad));
    int ny1 = cy + (int)((r_inner - 4) * sin(needleRad));
    int nx2 = cx + (int)((r_outer + 2) * cos(needleRad));
    int ny2 = cy + (int)((r_outer + 2) * sin(needleRad));
    d->drawLine(nx1, ny1, nx2, ny2, TFT_WHITE);
}

// ============================================
// Draw tick marks
// ============================================
void drawTicks(LGFX_GC9A01* d, int cx, int cy,
               const GaugeConfig& cfg, int numTicks) {

    int r_outer = cfg.arcRadius + 4;
    int r_inner = cfg.arcRadius - cfg.arcWidth - 2;

    for (int i = 0; i <= numTicks; i++) {
        float pct = (float)i / numTicks;
        float angle = (cfg.startAngle + pct * cfg.sweepAngle) * DEG2RAD;
        float cs = cos(angle);
        float sn = sin(angle);

        int x1 = cx + (int)(r_inner * cs);
        int y1 = cy + (int)(r_inner * sn);
        int x2 = cx + (int)(r_outer * cs);
        int y2 = cy + (int)(r_outer * sn);

        // Major ticks are longer
        bool major = (i % 2 == 0);
        if (major) {
            d->drawLine(x1, y1, x2, y2, TFT_LIGHTGREY);
        } else {
            int xm = cx + (int)((r_inner + 3) * cs);
            int ym = cy + (int)((r_inner + 3) * sn);
            d->drawLine(xm, ym, x2, y2, TFT_DARKGREY);
        }
    }
}

// ============================================
// Full gauge with label, value, and unit
// ============================================
void drawGauge(LGFX_GC9A01* d, int cx, int cy,
               float value, const GaugeConfig& cfg,
               const char* label, const char* unit,
               const char* valueFormat) {

    // Draw arc and ticks
    drawArc(d, cx, cy, value, cfg);
    drawTicks(d, cx, cy, cfg);

    // Label at top
    d->setTextDatum(middle_center);
    d->setTextSize(1.5);
    d->setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    d->drawString(label, cx, cy - 35);

    // Value in center (big)
    char valStr[16];
    snprintf(valStr, sizeof(valStr), valueFormat, value);
    d->setTextSize(3.5);
    uint32_t valColor = gaugeColor(value, cfg.warnVal, cfg.critVal);
    d->setTextColor(valColor, TFT_BLACK);
    d->drawString(valStr, cx, cy + 5);

    // Unit below value
    d->setTextSize(1.5);
    d->setTextColor(TFT_DARKGREY, TFT_BLACK);
    d->drawString(unit, cx, cy + 35);
}

// ============================================
// Mini gauge for multi-gauge layouts
// ============================================
void drawMiniGauge(LGFX_GC9A01* d, int cx, int cy,
                   float value, const GaugeConfig& cfg,
                   const char* label, const char* valueStr) {

    drawArc(d, cx, cy, value, cfg);

    // Label above
    d->setTextDatum(middle_center);
    d->setTextSize(1);
    d->setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    d->drawString(label, cx, cy - 12);

    // Value in center
    d->setTextSize(1.5);
    uint32_t valColor = gaugeColor(value, cfg.warnVal, cfg.critVal);
    d->setTextColor(valColor, TFT_BLACK);
    d->drawString(valueStr, cx, cy + 8);
}
