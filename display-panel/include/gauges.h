#pragma once

#include "displays.h"

// ============================================
// RPM-style arc gauge for round displays
// Draws a sweeping arc from startAngle to endAngle
// with colored segments (green → yellow → red)
// ============================================

struct GaugeConfig {
    float minVal;         // Minimum value (e.g., 0)
    float maxVal;         // Maximum value (e.g., 100)
    float warnVal;        // Yellow threshold
    float critVal;        // Red threshold
    int   arcRadius;      // Outer radius of arc
    int   arcWidth;       // Thickness of arc band
    int   startAngle;     // Start angle in degrees (135 = bottom-left)
    int   sweepAngle;     // Total sweep in degrees (270 = full gauge)
};

// Default gauge config (tachometer style: 7 o'clock to 5 o'clock)
static const GaugeConfig DEFAULT_GAUGE = {
    .minVal     = 0,
    .maxVal     = 100,
    .warnVal    = 70,
    .critVal    = 90,
    .arcRadius  = 105,
    .arcWidth   = 18,
    .startAngle = 135,
    .sweepAngle = 270
};

// Temperature gauge (20°C to 70°C for drives)
static const GaugeConfig TEMP_GAUGE = {
    .minVal     = 20,
    .maxVal     = 70,
    .warnVal    = 45,
    .critVal    = 55,
    .arcRadius  = 105,
    .arcWidth   = 18,
    .startAngle = 135,
    .sweepAngle = 270
};

// CPU gauge (0-100%)
static const GaugeConfig CPU_GAUGE = {
    .minVal     = 0,
    .maxVal     = 100,
    .warnVal    = 75,
    .critVal    = 90,
    .arcRadius  = 105,
    .arcWidth   = 18,
    .startAngle = 135,
    .sweepAngle = 270
};

// RAM gauge (0-100%)
static const GaugeConfig RAM_GAUGE = {
    .minVal     = 0,
    .maxVal     = 100,
    .warnVal    = 80,
    .critVal    = 95,
    .arcRadius  = 105,
    .arcWidth   = 18,
    .startAngle = 135,
    .sweepAngle = 270
};

// Small gauge for multi-gauge layouts (Pi rack screen)
static const GaugeConfig SMALL_GAUGE = {
    .minVal     = 20,
    .maxVal     = 85,
    .warnVal    = 65,
    .critVal    = 75,
    .arcRadius  = 42,
    .arcWidth   = 8,
    .startAngle = 135,
    .sweepAngle = 270
};

// Draw a full RPM-style gauge with value, label, and unit
void drawGauge(LGFX_GC9A01* d, int cx, int cy,
               float value, const GaugeConfig& cfg,
               const char* label, const char* unit,
               const char* valueFormat = "%.0f");

// Draw just the arc (for custom layouts)
void drawArc(LGFX_GC9A01* d, int cx, int cy,
             float value, const GaugeConfig& cfg);

// Draw tick marks around the gauge
void drawTicks(LGFX_GC9A01* d, int cx, int cy,
               const GaugeConfig& cfg, int numTicks = 9);

// Draw a mini gauge (for multi-gauge screens)
void drawMiniGauge(LGFX_GC9A01* d, int cx, int cy,
                   float value, const GaugeConfig& cfg,
                   const char* label, const char* valueStr);

// Color for a value given warn/crit thresholds
uint32_t gaugeColor(float value, float warn, float crit);
