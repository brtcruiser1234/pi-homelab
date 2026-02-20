#!/usr/bin/env python3
"""
Lightweight JSON stats API for M900 server.
Serves system health data for the rack display panel and web dashboard.

Usage: python3 m900-stats.py
Runs on port 9200, serves JSON at /stats and /health
"""

import json
import os
import time
import subprocess
from http.server import HTTPServer, BaseHTTPRequestHandler
import psutil

PORT = 9200


class StatsHandler(BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        pass  # Suppress request logs

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
        elif self.path == "/services":
            self._send_services()
        else:
            self.send_response(404)
            self.end_headers()

    def _send_stats(self):
        cpu_percent = psutil.cpu_percent(interval=0.5)
        cpu_freq = psutil.cpu_freq()
        mem = psutil.virtual_memory()
        disk = psutil.disk_usage("/")
        temps = psutil.sensors_temperatures()
        load1, load5, load15 = os.getloadavg()
        uptime_secs = time.time() - psutil.boot_time()

        # Get CPU temp (varies by hardware)
        cpu_temp = 0
        if temps:
            for name, entries in temps.items():
                if entries:
                    cpu_temp = entries[0].current
                    break

        stats = {
            "hostname": os.uname().nodename,
            "uptime_seconds": int(uptime_secs),
            "cpu": {
                "percent": cpu_percent,
                "cores": psutil.cpu_count(),
                "freq_mhz": int(cpu_freq.current) if cpu_freq else 0,
                "load_1m": round(load1, 2),
                "load_5m": round(load5, 2),
                "load_15m": round(load15, 2),
                "temp_c": round(cpu_temp, 1),
            },
            "memory": {
                "total_gb": round(mem.total / (1024**3), 1),
                "used_gb": round(mem.used / (1024**3), 1),
                "percent": mem.percent,
            },
            "disk": {
                "total_gb": round(disk.total / (1024**3), 1),
                "used_gb": round(disk.used / (1024**3), 1),
                "percent": disk.percent,
            },
            "network": {
                "bytes_sent": psutil.net_io_counters().bytes_sent,
                "bytes_recv": psutil.net_io_counters().bytes_recv,
            },
            "timestamp": int(time.time()),
        }

        self.send_response(200)
        self._cors_headers()
        self.end_headers()
        self.wfile.write(json.dumps(stats).encode())

    def _send_health(self):
        health = {"status": "ok", "hostname": os.uname().nodename}
        self.send_response(200)
        self._cors_headers()
        self.end_headers()
        self.wfile.write(json.dumps(health).encode())

    def _send_services(self):
        """Check which local services are responding."""
        import urllib.request

        services = {
            "jazz_stats": {"url": "http://localhost:8888", "up": False},
            "nhl_tracker": {"url": "http://localhost:3050", "up": False},
            "claudecad": {"url": "http://localhost:4000", "up": False},
            "cloudflared": {"url": None, "up": False},
        }

        # Check HTTP services
        for name, svc in services.items():
            if svc["url"]:
                try:
                    req = urllib.request.Request(svc["url"], method="HEAD")
                    resp = urllib.request.urlopen(req, timeout=2)
                    svc["up"] = resp.status < 400
                except Exception:
                    svc["up"] = False

        # Check cloudflared process
        try:
            result = subprocess.run(
                ["pgrep", "-f", "cloudflared"],
                capture_output=True, timeout=2
            )
            services["cloudflared"]["up"] = result.returncode == 0
        except Exception:
            pass

        self.send_response(200)
        self._cors_headers()
        self.end_headers()
        self.wfile.write(json.dumps(services).encode())


if __name__ == "__main__":
    server = HTTPServer(("0.0.0.0", PORT), StatsHandler)
    print(f"M900 Stats API running on port {PORT}")
    print(f"  /stats    - system stats")
    print(f"  /health   - health check")
    print(f"  /services - local service status")
    server.serve_forever()
