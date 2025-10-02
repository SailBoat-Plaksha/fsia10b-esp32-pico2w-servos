# FS-i6X → FS-iA10B → ESP32 → Pico 2 W → Servos

This project links a FlySky FS-i6X transmitter and FS-iA10B receiver to an ESP32, which decodes iBUS channel data and streams servo angles over Classic Bluetooth SPP to a Raspberry Pi Pico 2 W.  
The Pico then drives 4 servos safely with a watchdog that recenters them if the signal is lost.

---

## Setup

### 1. Bind transmitter and receiver
- Insert bind plug into RX “B/VCC”.
- Power RX → LED flashes.
- Hold **BIND** on FS-i6X → power on → RX LED solid.
- Remove bind plug.

### 2. Configure FS-i6X
- Menu → RX Setup → **Output Mode → Serial: i-BUS**

### 3. Wiring
- **iBUS to ESP32**:  
  RX iBUS (≈5 V) → Potentiometer divider (set wiper ~3.3 V) → ESP32 GPIO16 (RX2).  
  Optional: 1 kΩ series resistor + 100 kΩ pulldown for extra protection.  
- **Power**:  
  RX +5 V → ESP32 5V  
  RX GND → ESP32 GND  
- **Bluetooth link**:  
  ESP32 = SPP client, Pico 2 W = SPP server (`PICO2W_SPP`).  
- **Servos**:  
  Pico GP2–GP5 → Servo signals  
  External BEC +5 V → Servos V+  
  Common ground between RX, ESP32, Pico, and BEC.

### 4. Flashing order
1. Upload `Pico_iBUS_Motors.ino` to Pico 2 W (server).  
2. Upload `ESP32_iBUS_Motors.ino` to ESP32 (client).  
3. Power receiver and BEC.  

---

## Usage
- Open Serial Monitor (115200 baud).  
- ESP32 prints channel angles + `[LINK]` when connected.  
- Pico prints received values as `RX ANG: ...`.  
- Move sticks → servos follow.  
- If link drops >1.2 s, servos recenter to 90°.  

---

## License
MIT License – see [LICENSE](LICENSE).
