#!/usr/bin/env python3
"""
Simple HTTP wrapper for unraid-stats.sh
Runs the bash script and serves its JSON output.

Deploy to Unraid and run:
  python3 unraid-server.py

Or add to Unraid User Scripts to auto-start on boot.
Listens on port 9201.
"""

import json
import subprocess
import os
from http.server import HTTPServer, BaseHTTPRequestHandler

PORT = 9201
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
STATS_SCRIPT = os.path.join(SCRIPT_DIR, "unraid-stats.sh")


class UnraidHandler(BaseHTTPRequestHandler):
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
        try:
            result = subprocess.run(
                ["bash", STATS_SCRIPT],
                capture_output=True, text=True, timeout=10
            )
            output = result.stdout.strip()
            # Validate it's valid JSON
            json.loads(output)

            self.send_response(200)
            self._cors_headers()
            self.end_headers()
            self.wfile.write(output.encode())
        except Exception as e:
            error = json.dumps({"error": str(e)})
            self.send_response(500)
            self._cors_headers()
            self.end_headers()
            self.wfile.write(error.encode())

    def _send_health(self):
        health = json.dumps({"status": "ok", "hostname": "unraid"})
        self.send_response(200)
        self._cors_headers()
        self.end_headers()
        self.wfile.write(health.encode())


if __name__ == "__main__":
    server = HTTPServer(("0.0.0.0", PORT), UnraidHandler)
    print(f"Unraid Stats API running on port {PORT}")
    print(f"  /stats  - full system + drive stats")
    print(f"  /health - health check")
    server.serve_forever()
