# FlightRadar / ADS-B Receiver - Pi 3B

Tracks aircraft using an RTL-SDR dongle and feeds data to FlightRadar24 and FlightAware.

## Hardware

- Raspberry Pi 3B
- RTL-SDR USB dongle (with 1090MHz antenna)
- MicroSD card (16GB+ recommended)

## Software Stack

| Component | Purpose | Web UI |
|-----------|---------|--------|
| **readsb** | ADS-B decoder (replaces dump1090) | - |
| **tar1090** | Local aircraft map | `http://<pi-ip>/tar1090` |
| **graphs1090** | Performance graphs | `http://<pi-ip>/graphs1090` |
| **fr24feed** | FlightRadar24 feeder | `http://<pi-ip>:8754` |
| **piaware** | FlightAware feeder | - |

## Quick Start

### 1. Flash Raspberry Pi OS Lite

Use Raspberry Pi Imager:
- OS: **Raspberry Pi OS Lite (64-bit)** (or 32-bit for Pi 3B)
- Set hostname: `flight-radar`
- Enable SSH
- Set username/password
- Configure WiFi (if not using ethernet)

### 2. First Boot Setup

SSH into the Pi and run:

```bash
sudo apt update && sudo apt upgrade -y
```

### 3. Run the Setup Script

Copy the setup script to the Pi and run it:

```bash
scp setup.sh pi@flight-radar.local:~/
ssh pi@flight-radar.local
chmod +x setup.sh
sudo ./setup.sh
```

The script will install everything in order. You'll be prompted for:
- FlightRadar24 signup (email + sharing key)
- FlightAware claim info
- Your latitude/longitude/altitude

### 4. Get Your Coordinates

You'll need your home coordinates for accurate tracking:
- Go to https://www.latlong.net/
- Find your location
- Note: latitude, longitude, and altitude (in feet)

## Web Interfaces

After setup, access these from any browser on your network:

| URL | What |
|-----|------|
| `http://flight-radar:8080` | tar1090 - live aircraft map |
| `http://flight-radar/graphs1090` | Performance graphs |
| `http://flight-radar:8754` | FR24 feeder status |

## Feeding Benefits

- **FlightRadar24:** Free Business subscription ($499/yr value)
- **FlightAware:** Free Enterprise account ($899/yr value)

## Files

```
flight-radar/
├── README.md          # This file
├── setup.sh           # Main installation script
├── config/
│   └── readsb.conf    # readsb configuration template
└── scripts/
    └── check-status.sh # Quick health check script
```

## Antenna Tips

- Mount the antenna as high as possible (roof/attic ideal)
- 1090MHz antenna works best (comes with most RTL-SDR kits)
- Line of sight = more planes. Every obstacle reduces range
- Typical range: 100-200 nautical miles with good antenna placement
