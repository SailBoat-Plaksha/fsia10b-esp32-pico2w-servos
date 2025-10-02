# FS-i6X → FS-iA10B → ESP32 → Pico 2 W → Servos

This project connects a FlySky FS-i6X transmitter and FS-iA10B receiver to an ESP32, which decodes iBUS channel data and sends servo angles over Classic Bluetooth SPP to a Raspberry Pi Pico 2 W.  
The Pico receives the data and drives 4 servos.  

**Flow:**  
FS-i6X (TX) → FS-iA10B (RX) → ESP32 (client) → Bluetooth → Pico 2 W (server) → Servos

## License
MIT License – see [LICENSE](LICENSE).
