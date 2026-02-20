# Uptime Kuma - Service Monitor

Monitors all your homelab services and alerts you when something goes down.

## Hardware

- Raspberry Pi 3B (32GB SD card)
- Ethernet connection

## Quick Start

1. Flash Pi OS Lite, SSH in
2. Run the shared first-boot script: `sudo ./first-boot.sh`
3. Run the Uptime Kuma setup: `sudo ./setup.sh`
4. Visit `http://uptime-kuma.local:3001` and create admin account
5. Add monitors for your services

## Recommended Monitors

### HTTP Monitors
| Service | URL | Interval |
|---------|-----|----------|
| Jazz-Stats | `http://<m900-ip>:8888` | 60s |
| NHL Tracker | `http://<m900-ip>:3050` | 60s |
| ClaudeCAD | `http://<m900-ip>:4000` | 60s |
| Pi-hole Admin | `http://pihole.local/admin` | 60s |
| FlightRadar Map | `http://flight-radar.local/tar1090` | 60s |
| FR24 Feeder | `http://flight-radar.local:8754` | 60s |

### Ping Monitors
| Host | Address | Interval |
|------|---------|----------|
| M900 Server | `<m900-ip>` | 30s |
| Unraid NAS | `10.1.10.193` | 30s |
| Pi-hole | `pihole.local` | 30s |
| Flight Radar | `flight-radar.local` | 30s |

### DNS Monitors
| Domain | Check |
|--------|-------|
| jazzlive.taylorshome.cc | DNS resolves correctly |
| nhlteams.taylorshome.cc | DNS resolves correctly |
| claudecad.taylorshome.cc | DNS resolves correctly |

## Dashboard

Access at `http://uptime-kuma.local:3001`

- Real-time status for all services
- Response time graphs
- Uptime percentage history
- Notification support (email, Slack, Discord, etc.)

## Service Management

```bash
sudo systemctl status uptime-kuma    # Check status
sudo systemctl restart uptime-kuma   # Restart
sudo systemctl stop uptime-kuma      # Stop
journalctl -u uptime-kuma -f         # View logs
```

## Data

- SQLite database at `/opt/uptime-kuma/data/kuma.db`
- Backup: `cp /opt/uptime-kuma/data/kuma.db ~/kuma-backup.db`
