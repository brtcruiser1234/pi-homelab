#!/usr/bin/env python3
"""
Lightweight JSON stats API for Raspberry Pi.
Serves system health data for the rack display panel and web dashboard.

Usage: python3 pi-stats.py
Runs on port 9200, serves JSON at /stats and /health

Works on Pi 3B with minimal dependencies (no psutil needed).
"""

import json
import os
import time
from http.server import HTTPServer, BaseHTTPRequestHandler

PORT = 9200


def read_file(path):
    try:
        with open(path) as f:
            return f.read().strip()
    except Exception:
        return None


def get_cpu_temp():
    val = read_file("/sys/class/thermal/thermal_zone0/temp")
    if val:
        return round(int(val) / 1000.0, 1)
    return 0


def get_cpu_percent():
    """Read CPU usage from /proc/stat (simple 1-second sample)."""
    def read_stat():
        with open("/proc/stat") as f:
            line = f.readline()
        parts = line.split()
        idle = int(parts[4])
        total = sum(int(p) for p in parts[1:])
        return idle, total

    idle1, total1 = read_stat()
    time.sleep(0.5)
    idle2, total2 = read_stat()

    idle_delta = idle2 - idle1
    total_delta = total2 - total1
    if total_delta == 0:
        return 0
    return round((1 - idle_delta / total_delta) * 100, 1)


def get_memory():
    meminfo = {}
    with open("/proc/meminfo") as f:
        for line in f:
            parts = line.split()
            meminfo[parts[0].rstrip(":")] = int(parts[1])

    total = meminfo.get("MemTotal", 0)
    available = meminfo.get("MemAvailable", 0)
    used = total - available
    percent = round((used / total) * 100, 1) if total > 0 else 0

    return {
        "total_mb": round(total / 1024, 0),
        "used_mb": round(used / 1024, 0),
        "percent": percent,
    }


def get_disk():
    st = os.statvfs("/")
    total = st.f_blocks * st.f_frsize
    free = st.f_bfree * st.f_frsize
    used = total - free
    percent = round((used / total) * 100, 1) if total > 0 else 0

    return {
        "total_gb": round(total / (1024**3), 1),
        "used_gb": round(used / (1024**3), 1),
        "percent": percent,
    }


def get_uptime():
    val = read_file("/proc/uptime")
    if val:
        return int(float(val.split()[0]))
    return 0


class StatsHandler(BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        pass

    def _cors_headers(self):
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, OPTIONS")
        self.send_header("Content-Type", "application/json")

    def do_OPTIONS(self):
        self.send_response(200)
        self._cors_headers()
        self.end_headers()

    def do_GET(self):
        if self.path == "/stats":
            self._send_stats()
        elif self.path == "/health":
            self._send_health()
        else:
            self.send_response(404)
            self.end_headers()

    def _send_stats(self):
        stats = {
            "hostname": os.uname().nodename,
            "uptime_seconds": get_uptime(),
            "cpu": {
                "percent": get_cpu_percent(),
                "temp_c": get_cpu_temp(),
                "cores": os.cpu_count(),
            },
            "memory": get_memory(),
            "disk": get_disk(),
            "timestamp": int(time.time()),
        }

        self.send_response(200)
        self._cors_headers()
        self.end_headers()
        self.wfile.write(json.dumps(stats).encode())

    def _send_health(self):
        health = {
            "status": "ok",
            "hostname": os.uname().nodename,
            "cpu_temp": get_cpu_temp(),
        }
        self.send_response(200)
        self._cors_headers()
        self.end_headers()
        self.wfile.write(json.dumps(health).encode())


if __name__ == "__main__":
    server = HTTPServer(("0.0.0.0", PORT), StatsHandler)
    print(f"Pi Stats API running on port {PORT}")
    print(f"  Hostname: {os.uname().nodename}")
    print(f"  /stats  - system stats")
    print(f"  /health - health check")
    server.serve_forever()
