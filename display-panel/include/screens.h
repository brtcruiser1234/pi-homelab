#pragma once

#include "displays.h"
#include <ArduinoJson.h>

// --- Screen drawing functions ---

// Screen 0: Clock + Date
void drawClock(int idx);

// Screen 1: Weather (OpenWeatherMap)
void drawWeather(int idx);

// Screen 2: Flight Radar stats
void drawFlights(int idx);

// Screen 3: Service status (up/down indicators)
void drawServices(int idx);

// Screen 4: System stats (Pi temps, M900 load)
void drawStats(int idx);

// Screen 5: Sports scores (Jazz / Mammoth)
void drawSports(int idx);

// --- Data fetch functions (called on intervals) ---
void fetchWeather();
void fetchFlights();
void fetchServices();
void fetchStats();
void fetchSports();

// --- Boot splash ---
void drawBootSplash(int idx, const char* label);
