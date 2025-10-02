# FS-i6X → FS-iA10B → ESP32 → Pico 2 W → Servos

This project links a FlySky FS-i6X transmitter and FS-iA10B receiver to an ESP32, which decodes **iBUS** channel data and streams servo angles over Classic Bluetooth SPP to a Raspberry Pi Pico 2 W.  
The Pico then drives up to 4 servos with a watchdog that recenters them if the signal is lost.

---

## Hardware Required
- FlySky **FS-i6X** transmitter  
- FlySky **FS-iA10B** receiver (set to *Serial i-BUS*)  
- **ESP32 DevKit** (WROOM/WROVER, must support Classic BT)  
- **Raspberry Pi Pico 2 W**  
- Up to 4x 5 V servos  
- **5 V BEC** (to power servos safely, not through USB)  
- Potentiometer (used as voltage divider, fixed wiper ≈ 3.3 V)  
- Short-circuit jumpers (to make a DIY bind plug)  
- Hookup wires and common ground  

---

## Step 1: Bind TX and RX
The FS-iA10B requires a bind plug during the first bind:  
- A **bind plug** is just the signal and ground pins shorted. You can make one with **short-circuit jumper wires** plugged into the RX “B/VCC” port.  
- Insert bind plug → power the RX → LED flashes.  
- Hold **BIND** on the FS-i6X and power it on → RX LED goes solid.  
- Remove the bind plug → binding complete.

---

## Step 2: Configure FS-i6X
- On TX: **Menu → RX Setup → Output Mode → Serial: i-BUS**

---

## Step 3: Wiring

### RX → ESP32 (iBUS)
- FS-iA10B iBUS signal (~5 V) → **potentiometer divider** → ESP32 GPIO16 (RX2)  
- Potentiometer must be set so the **wiper is ~3.3 V** when RX outputs ~5 V.  
  - Fix this setting permanently (glue or tape the pot so it can’t drift).  
- Optional safety:  
  - 1 kΩ series resistor on RX2  
  - 100 kΩ pulldown to GND  

### RX Power
- FS-iA10B +5 V → ESP32 5V  
- FS-iA10B GND → ESP32 GND  

### ESP32 ↔ Pico 2 W
- Classic Bluetooth SPP link  
- ESP32 = client  
- Pico 2 W = server (`PICO2W_SPP`)  

### Pico → Servos
- GP2 → Servo1 signal  
- GP3 → Servo2 signal  
- GP4 → Servo3 signal  
- GP5 → Servo4 signal  

### Servo Power
- BEC +5 V → Servos V+  
- BEC GND → Servos GND → Pico GND → ESP32 GND → RX GND (**all grounds common**)  
- Add bulk capacitors: ≥470 µF near servos, ≥100 µF near ESP32  

---

## Step 4: Flashing Order
1. Upload `Pico_iBUS_Motors.ino` to Pico 2 W → advertises as `PICO2W_SPP`  
2. Upload `ESP32_iBUS_Motors.ino` to ESP32 → connects to Pico via MAC  
3. Power the receiver and servos  

---

## Step 5: Usage
- Open Serial Monitor (115200 baud)  
- ESP32 prints channel angles (`a0:.. a1:.. a2:.. a3:.. [LINK]`)  
- Pico prints received values as `RX ANG: ...`  
- Move FS-i6X sticks → servos follow  
- If the link drops >1.2 s → all servos recenter to 90°  

---

## Notes
- Only **CH3 is inverted** in code (others normal).  
- The potentiometer divider must be **fixed permanently** so it never drifts above 3.3 V.  
- Without a proper common ground, servos will jitter or not move.  
- Always power servos from a **BEC**, not from Pico or ESP32 USB.

---

## License
MIT License – see [LICENSE](LICENSE).
