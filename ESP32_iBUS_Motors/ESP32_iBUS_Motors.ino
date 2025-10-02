#include <HardwareSerial.h>
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error "Classic Bluetooth is not enabled in this ESP32 build."
#endif

// ---- iBUS on Serial2 ----
HardwareSerial IBUS(2);
static const int PIN_RX2 = 16, PIN_TX2 = 17;
static const uint8_t IBUS_LEN = 0x20, IBUS_CMD = 0x40;
static const int IBUS_CHN = 14;

uint8_t buf[34];
int ch[IBUS_CHN];

// ---- Classic SPP ----
BluetoothSerial SerialBT;

// >>> Pico 2 W MAC (from your scan) <<<
uint8_t PICO_MAC[6] = {0x2C, 0xCF, 0x67, 0xD1, 0xE3, 0x46};

// ---- helpers ----
uint16_t rd16(const uint8_t* p) { return p[0] | (p[1] << 8); }
int clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }
int mapAngle(int v) { v = clampi(v, 1000, 2000); return map(v, 1000, 2000, 0, 180); }

bool readIbusFrame() {
  while (IBUS.available()) {
    if ((uint8_t)IBUS.peek() != IBUS_LEN) { IBUS.read(); continue; }
    if (IBUS.available() < 32) return false;
    for (int i = 0; i < 32; i++) buf[i] = IBUS.read();
    if (buf[0] != IBUS_LEN || buf[1] != IBUS_CMD) continue;

    uint32_t sum = 0; for (int i = 0; i < 30; i++) sum += buf[i];
    uint16_t cksum = 0xFFFF - (sum & 0xFFFF);
    uint16_t got   = rd16(&buf[30]);
    if (cksum != got) continue;

    for (int i = 0; i < IBUS_CHN; i++) ch[i] = rd16(&buf[2 + i * 2]);
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  IBUS.begin(115200, SERIAL_8N1, PIN_RX2, PIN_TX2);

  // Start Classic BT in master/client mode
  SerialBT.begin("ESP32_SPP_CLIENT", /*isMaster=*/true);
  Serial.println("ESP32 SPP client boot.");
}

void loop() {
  static unsigned long tLastTx = 0;
  static unsigned long nextRetryMs = 0;
  static uint32_t backoff = 1000; // ms (1s..5s)

  // Default angles (Pico watchdog recenters anyway)
  static int a0 = 90, a1 = 90, a2 = 90, a3 = 90;

  unsigned long now = millis();

  // Read iBUS → map to angles (CH3 inverted)
  if (readIbusFrame()) {
    a0 = mapAngle(ch[0]);          // CH1 normal
    a1 = mapAngle(ch[1]);          // CH2 normal
    a2 = 180 - mapAngle(ch[2]);    // CH3 inverted
    a3 = mapAngle(ch[3]);          // CH4 normal
  }

  // Connect by MAC with exponential backoff
  if (!SerialBT.connected() && now >= nextRetryMs) {
    bool ok = SerialBT.connect(PICO_MAC);
    Serial.println(ok ? "BT: connected" : "BT: retry...");
    nextRetryMs = now + backoff;
    if (!ok) backoff = (backoff < 5000) ? (backoff << 1) : 5000;
    else backoff = 1000;
  }

  // Send packet + print debug every 50 ms
  if (now - tLastTx >= 50) {
    if (SerialBT.connected()) {
      SerialBT.printf("ANG:%d,%d,%d,%d\n", a0, a1, a2, a3);
    }
    Serial.printf("a0:%d a1:%d a2:%d a3:%d %s\n",
                  a0, a1, a2, a3, SerialBT.connected() ? "[LINK]" : "[RETRY]");
    tLastTx = now;
  }

  delay(3);
}
