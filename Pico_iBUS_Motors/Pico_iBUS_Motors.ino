#include <BluetoothSerial.h>
#include <Servo.h>

static const uint8_t SERVO_PINS[4] = {2, 3, 4, 5};
static const int SERVO_NEUTRAL = 90;
static const unsigned long RX_TIMEOUT = 1200; // ms

Servo servos[4];
int target[4] = {90, 90, 90, 90};
unsigned long lastRx = 0;

char lineBuf[64];
size_t lineLen = 0;

int clamp180(int x) { return x < 0 ? 0 : (x > 180 ? 180 : x); }

void centerAll() {
  for (int i = 0; i < 4; i++) target[i] = SERVO_NEUTRAL;
}

void parseLine(const char* s) {
  int a0, a1, a2, a3;
  if (sscanf(s, "ANG:%d,%d,%d,%d", &a0, &a1, &a2, &a3) == 4) {
    target[0] = clamp180(a0);
    target[1] = clamp180(a1);
    target[2] = clamp180(a2);
    target[3] = clamp180(a3);
    lastRx = millis();
    Serial.printf("RX ANG: %d,%d,%d,%d\n", a0, a1, a2, a3);
  }
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) {
    servos[i].attach(SERVO_PINS[i]);
    servos[i].write(SERVO_NEUTRAL);
  }
  SerialBT.begin("PICO2W_SPP");  // Advertises this name
  Serial.println("Pico 2 W SPP server ready.");
  lastRx = millis();
}

void loop() {
  // Read SPP by lines
  while (SerialBT.available()) {
    char c = (char)SerialBT.read();
    if (c == '\r') continue;
    if (c == '\n') {
      lineBuf[lineLen] = '\0';
      parseLine(lineBuf);
      lineLen = 0;
    } else if (lineLen < sizeof(lineBuf) - 1) {
      lineBuf[lineLen++] = c;
    } else {
      lineLen = 0; // overflow safety
    }
  }

  // Watchdog: recenter if stream lost
  if (millis() - lastRx > RX_TIMEOUT) centerAll();

  // Drive servos
  for (int i = 0; i < 4; i++) servos[i].write(target[i]);
}
