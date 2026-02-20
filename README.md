# Pi Homelab

Raspberry Pi homelab project for a 10" Lab Rax rack setup.

## Hardware

| # | Board | Role | Status |
|---|-------|------|--------|
| 1 | Raspberry Pi 3B | FlightRadar / ADS-B Receiver | Planning |
| 2 | Raspberry Pi 3B | Display Panel Driver (6x round TFTs) | Planning |
| 3 | Raspberry Pi 3B | TBD | Planning |
| 4 | Raspberry Pi 3B | TBD | Planning |

## Rack Setup

- **Rack:** Lab Rax 10" 3D-printable modular rack
- **Mount:** Custom 3D printed blade-style rack mount
- **Printer:** Bambu Lab

## Project Ideas

### Confirmed
- **FlightRadar24 / ADS-B Receiver** - Track aircraft with an RTL-SDR dongle + Pi

### Confirmed
- **1U Display Panel** - 6x 1.28" round GC9A01 TFTs in a custom rack-mounted faceplate ([detailed plan](docs/display-panel-plan.md))

### Under Consideration
- Pi-hole DNS ad blocker
- Home Assistant
- Network monitoring (Uptime Kuma, Grafana)
- Media server (Jellyfin/Plex)
- Print server (OctoPrint/CUPS)

## Directory Structure

```
pi-homelab/
├── README.md           # This file
├── 3d-prints/          # STL files and OpenSCAD sources for rack mounts
├── display-panel/      # 1U display panel (6x round TFTs) software
├── flight-radar/       # FlightRadar24 setup scripts and config
├── docs/               # Planning docs, wiring diagrams, etc.
└── scripts/            # Setup/provisioning scripts
```

## Rack Specs (Lab Rax 10")

- 44.45mm per rack unit (U)
- 236.525mm horizontal hole spacing
- 222mm usable width
- M6x10mm button head screws + brass inserts
- See [3d-prints/](3d-prints/) for mount designs
