# Pi Homelab

Raspberry Pi homelab project for a 10" Lab Rax rack setup.

## Hardware

| # | Board | Role | Status |
|---|-------|------|--------|
| 1 | Raspberry Pi 3B | FlightRadar / ADS-B Receiver | Setup ready |
| 2 | Raspberry Pi 3B | Pi-hole (DNS ad blocker) | Planning |
| 3 | Raspberry Pi 3B | Uptime Kuma (service monitor) | Planning |
| 4 | Raspberry Pi 3B | Spare / experiments | - |
| - | ESP32 | Display Panel (6x round TFTs) | Planning |

## Rack Layout

```
┌─────────────────────────────┐
│  1U  │ Display Panel (ESP32)│  6x round 1.28" TFTs
├──────┴──────────────────────┤
│  2U  │ 4x Raspberry Pi 3B  │  Blade-style rack mount
│      │ (flight, pihole,     │
│      │  uptime, spare)      │
├──────┴──────────────────────┤
│  1U  │ USW Flex switches    │  Dual switch bracket
└─────────────────────────────┘
```

- **Rack:** Lab Rax 10" 3D-printable modular rack
- **Mount:** Custom 3D printed blade-style rack mount
- **Printer:** Bambu Lab

## Projects

| Project | Directory | Docs |
|---------|-----------|------|
| FlightRadar / ADS-B | [flight-radar/](flight-radar/) | [README](flight-radar/README.md) |
| Display Panel (1U) | [display-panel/](display-panel/) | [Plan](docs/display-panel-plan.md) |
| Pi-hole | pihole/ | TBD |
| Uptime Kuma | uptime-kuma/ | TBD |

## Directory Structure

```
pi-homelab/
├── README.md           # This file
├── 3d-prints/          # STL files and OpenSCAD sources for rack mounts
├── display-panel/      # 1U display panel (ESP32 + 6x round TFTs)
├── flight-radar/       # FlightRadar24 setup scripts and config
├── pihole/             # Pi-hole setup (TBD)
├── uptime-kuma/        # Uptime Kuma setup (TBD)
├── docs/               # Planning docs, wiring diagrams, etc.
└── scripts/            # Shared setup/provisioning scripts
```

## Rack Specs (Lab Rax 10")

- 44.45mm per rack unit (U)
- 236.525mm horizontal hole spacing
- 222mm usable width
- M6x10mm button head screws + brass inserts
- See [3d-prints/](3d-prints/) for mount designs
