# Rack-Mounted Display Panel - 1U

## Concept
6x round 1.28" GC9A01 TFT displays mounted in a custom 3D-printed 1U faceplate for the Lab Rax 10" rack. Driven by a Raspberry Pi 3B located in the separate 2U Pi rack mount, connected via custom SPI cables.

## Hardware

### Displays
- **Model:** 1.28" round TFT LCD (GC9A01 driver)
- **Resolution:** 240x240 pixels
- **Interface:** SPI
- **Quantity:** 6
- **Status:** Already owned

### Controller
- **Board:** Raspberry Pi 3B (in separate 2U rack mount)
- **Connection:** Custom SPI cables (crimped slide-on connectors, repurposed 3D printer wire)
- **SPI Bus:** Shared MOSI/SCLK/DC/RST, individual CS per display

### Wiring (Pi 3B SPI to 6x GC9A01)
Shared lines:
- **MOSI (SDA):** GPIO 10 (SPI0_MOSI)
- **SCLK:** GPIO 11 (SPI0_SCLK)
- **DC:** Any GPIO (e.g., GPIO 24)
- **RST:** Any GPIO (e.g., GPIO 25)
- **VCC:** 3.3V
- **GND:** GND

Individual chip selects (one per display):
- Display 1: GPIO 8 (CE0)
- Display 2: GPIO 7 (CE1)
- Display 3: GPIO 5
- Display 4: GPIO 6
- Display 5: GPIO 12
- Display 6: GPIO 13

> **Note:** Pi 3B has 2 hardware CE pins (CE0, CE1). The other 4 CS pins will be bit-banged via software SPI or GPIO toggling. Libraries like `luma.lcd` or `Pillow` + `spidev` handle this.

## Dimensions & Fit

### 1U Panel Specs
- **Panel height:** 44.45mm (1U)
- **Usable width:** 222mm (between rack posts)
- **Ear width:** ~7.25mm per side (for screw holes)
- **Total panel width:** 236.525mm (ear to ear)

### Display Cutouts
- **Display diameter:** 32.5mm (1.28")
- **Cutout diameter:** ~33mm (with tolerance)
- **6 displays:** 6 x 33mm = 198mm
- **5 gaps between displays:** 5 x 3mm = 15mm
- **2 edge margins:** 2 x 4.5mm = 9mm
- **Total:** 222mm (exact fit)

### Vertical Fit
- Panel height: 44.45mm
- Display: 32.5mm
- Margin: ~6mm per side (top/bottom)
- **Result:** Comfortable fit

## 3D Print Design (OpenSCAD)

### Faceplate
- 1U panel with 6 circular cutouts
- Mounting ears with 3x M6 holes per side (EIA-310 pattern)
- Snap-fit or press-fit bezels for each display
- Cable routing channel on the back

### Optional: Rear cover
- Enclose the back of the displays
- Strain relief for the SPI cable bundle
- Ventilation slots

## Software Stack

### Display Driver Options
1. **luma.lcd** (Python) - mature library, supports GC9A01
2. **Pillow + spidev** - lower level, more control
3. **fbcp-ili9341** - framebuffer copy, works with some round TFTs
4. **Custom Python** - direct SPI commands

### Display Content Ideas
- Clock / date
- System stats (CPU, RAM, temp for each Pi)
- Network status / IP addresses
- Flight radar aircraft count (from ADS-B Pi)
- Weather
- Custom graphics / logos

## Parts List
| Item | Qty | Status |
|------|-----|--------|
| 1.28" GC9A01 round TFT | 6 | Have |
| Raspberry Pi 3B | 1 | Have |
| Dupont/JST crimp connectors | ~40 | Have (from printer wire) |
| Wire (repurposed) | ~1m | Have |
| 3D printed 1U faceplate | 1 | To design |
| M6x10mm screws | 6 | Have (from rack hardware) |

## Next Steps
1. [ ] Design 1U faceplate in OpenSCAD
2. [ ] Print and test-fit one display in the panel
3. [ ] Wire up 1 display to Pi 3B as proof of concept
4. [ ] Get display driver working (Python + GC9A01)
5. [ ] Wire all 6 displays
6. [ ] Build dashboard software
7. [ ] Make custom cable harness with crimped connectors
