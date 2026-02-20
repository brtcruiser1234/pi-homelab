#pragma once

#include "displays.h"
#include "gauges.h"
#include <ArduinoJson.h>

// --- Screen drawing functions ---

// Screen 0: Unraid health (drive temps, array, storage)
void drawUnraid(int idx);

// Screen 1: M900 health (CPU/RAM/disk gauges)
void drawM900(int idx);

// Screen 2: Pi rack health (4 mini gauges for each Pi)
void drawPiHealth(int idx);

// Screen 3: Service status (up/down indicators)
void drawServices(int idx);

// Screen 4: Custom stats (configurable gauge)
void drawCustom(int idx);

// Screen 5: Clock + date (minimal)
void drawClock(int idx);

// --- Data fetch functions ---
void fetchUnraid();
void fetchM900();
void fetchPiHealth();
void fetchServices();
void fetchCustom();

// --- Boot splash ---
void drawBootSplash(int idx, const char* label);
