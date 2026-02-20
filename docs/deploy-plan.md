# Deployment Plan

Everything that's left to do, in order.

## Already Done
- [x] M900 stats API (`m900-stats.py` on port 9200, systemd service)
- [x] Web dashboard (`server.js` on port 9300, systemd service)
- [x] Cloudflare tunnel route (`rackstatus.taylorshome.cc`)
- [x] SD cards flashed (4x Pi OS Lite, all with SSH keys)

## When You Get Home

### Step 1: Boot all 4 Pis and get IPs
```bash
# Plug Pis into ethernet and power
# Find them on your network:
ping flight-radar.local
ping uptime-kuma.local
ping pi-spare.local

# Or check your router's DHCP leases for new devices
# Write down the IPs
```

### Step 2: Run first-boot on all 4 Pis
From M900, for each Pi:
```bash
scp ~/pi-homelab/scripts/first-boot.sh brandon@flight-radar.local:~/
ssh brandon@flight-radar.local "sudo ./first-boot.sh"
```
Repeat for `uptime-kuma`, `pi-spare` (x2).

This does: apt update, installs common tools, sets timezone (Mountain), enables firewall, disables SSH password auth, reduces swap.

### Step 3: Deploy Pi stats API to all 4 Pis
Each Pi needs `pi-stats.py` running so the dashboard and ESP32 can read their health:
```bash
# For each Pi:
scp ~/pi-homelab/stats-api/pi-stats.py brandon@flight-radar.local:~/
ssh brandon@flight-radar.local "nohup python3 ~/pi-stats.py > /tmp/pi-stats.log 2>&1 &"
```
Repeat for all 4. Later you can make this a systemd service on each.

### Step 4: Deploy flight-radar setup
```bash
scp ~/pi-homelab/flight-radar/setup.sh brandon@flight-radar.local:~/
ssh brandon@flight-radar.local
sudo ./setup.sh
# Will prompt for: latitude, longitude, altitude
# Will prompt for: FlightRadar24 signup, FlightAware signup
```
**You'll need:** Your home coordinates (https://www.latlong.net/) and the RTL-SDR dongle plugged in.

### Step 5: Deploy Uptime Kuma setup
```bash
scp ~/pi-homelab/uptime-kuma/setup.sh brandon@uptime-kuma.local:~/
ssh brandon@uptime-kuma.local
sudo ./setup.sh
```
Then visit `http://uptime-kuma.local:3001`, create admin account, and add monitors.

### Step 6: Deploy Unraid stats API
```bash
scp ~/pi-homelab/stats-api/unraid-stats.sh brandon@10.1.10.193:~/
scp ~/pi-homelab/stats-api/unraid-server.py brandon@10.1.10.193:~/
# Note: Unraid is SCP only, can't SSH interactively
# You'll need to start unraid-server.py via the Unraid web UI (User Scripts plugin)
# or add it to /boot/config/go for auto-start
```

### Step 7: Update IPs in config files
Once all Pis are online and you have their IPs:

**ESP32 firmware** (`display-panel/include/config.h`):
- Set M900_IP
- Set Pi hostnames/IPs

**Web dashboard** (`web-dashboard/server.js`):
- Set M900_IP (currently 127.0.0.1, which works since it's on M900)
- Update Pi hostnames if .local doesn't resolve

Restart dashboard: `sudo systemctl restart rack-dashboard`

### Step 8: Flash ESP32-S3
On a computer with PlatformIO installed:
```bash
cd ~/pi-homelab/display-panel
pio run -t upload
```
Hold BOOT on the S3, press RESET, release BOOT to enter download mode.

On first boot, connect phone to "RackDisplay" WiFi AP and configure home WiFi.

### Step 9: Wire displays to ESP32
Connect 6x GC9A01 displays to ESP32-S3 per the wiring table in `display-panel/README.md`. Shared SPI bus + 6 individual CS pins.

### Step 10: Print the 1U faceplate
Design in OpenSCAD, print on Bambu. 6 circular cutouts (33mm) across 222mm usable width.

## Order of Priority
1. Boot Pis, first-boot, deploy pi-stats (gets dashboard showing Pi health immediately)
2. Unraid stats API (gets dashboard showing drive temps)
3. Flight radar setup (fun, standalone)
4. Uptime Kuma (monitoring)
5. ESP32 flash + wiring (display panel)
6. 3D print faceplate (last, needs design work)
