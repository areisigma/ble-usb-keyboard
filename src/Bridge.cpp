#include "Bridge.h"
#include "NVSUtils.h"
#include <hid_usage_keyboard.h>

uint8_t Bridge::_currentSlot = 0;
BLEManager Bridge::_bleManager;
Preferences Bridge::_preferences;

static void readBattery(float &outVoltage, uint8_t &outPercent) {
  const int NUM_SAMPLES = 16;
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(BATTERY_ADC_PIN);
    delay(2);
  }
  float raw   = sum / (float)NUM_SAMPLES;
  float v_adc = raw * 3.3f / 4095.0f;
  float v_bat = v_adc * (BATTERY_R1 + BATTERY_R2) / BATTERY_R2 + BATTERY_VOLTAGE_OFFSET;

  Serial.printf("[Battery] ADC=%.0f, V_adc=%.3fV, V_bat=%.3fV\n", raw, v_adc, v_bat);

  outVoltage = v_bat;

  // Li-ion/LiPo voltage-to-percent lookup table
  const float   voltages[] = {4.2f, 3.9f, 3.7f, 3.5f, 3.0f};
  const uint8_t percents[] = {100,   75,   50,   25,    0};
  const int TABLE_SIZE = 5;

  if (v_bat >= voltages[0])           { outPercent = 100; return; }
  if (v_bat <= voltages[TABLE_SIZE-1]){ outPercent = 0;   return; }

  for (int i = 0; i < TABLE_SIZE - 1; i++) {
    if (v_bat <= voltages[i] && v_bat >= voltages[i + 1]) {
      float t = (v_bat - voltages[i + 1]) / (voltages[i] - voltages[i + 1]);
      outPercent = (uint8_t)(percents[i + 1] + t * (float)(percents[i] - percents[i + 1]));
      return;
    }
  }
  outPercent = 0;
}

static void printVoltageChart(const BatterySample *samples, uint8_t count, uint8_t start) {
  const float V_MIN = 3.0f, V_MAX = 4.2f;
  const int   ROWS  = 10;
  const float step  = (V_MAX - V_MIN) / ROWS;

  int sampleRow[MAX_BATTERY_SAMPLES];
  for (int i = 0; i < count; i++) {
    uint8_t idx = (start + i) % MAX_BATTERY_SAMPLES;
    int r = (int)((V_MAX - samples[idx].voltage) / step);
    if (r < 0)     r = 0;
    if (r >= ROWS) r = ROWS - 1;
    sampleRow[i] = r;
  }

  Serial.println("\n--- Voltage Chart (oldest left, newest right) ---");
  for (int row = 0; row < ROWS; row++) {
    float rowV = V_MAX - row * step;
    Serial.printf("%.2fV |", rowV);
    for (int i = 0; i < count; i++) {
      Serial.print(sampleRow[i] == row ? '*' : ' ');
    }
    Serial.println();
  }
  Serial.printf("%.2fV +", V_MIN);
  for (int i = 0; i < count; i++) Serial.print('-');
  Serial.println();
  if (count > 0) {
    uint8_t sN = (uint8_t)((start + count - 1) % MAX_BATTERY_SAMPLES);
    Serial.printf("       t_first=+%u min  t_last=+%u min  (%d samples x ~5 min)\n",
                  samples[start].minutes, samples[sN].minutes, count);
  }
  Serial.println("-------------------------------------------------");
}

static void printBatteryStatus() {
  float   voltage;
  uint8_t pct;
  readBattery(voltage, pct);

  Serial.println("\n=== BATTERY STATUS ===");
  Serial.printf("Current:  %.3fV  %d%%\n", voltage, pct);

  BatterySample history[MAX_BATTERY_SAMPLES] = {};
  uint8_t count = 0, head = 0;
  NVSUtils::loadBatteryHistory(history, count, head);

  Serial.printf("Stored:   %d / %d samples\n", count, MAX_BATTERY_SAMPLES);

  if (count == 0) {
    Serial.println("(no history yet - first save after 5 min of uptime)");
    Serial.println("======================");
    return;
  }

  uint8_t start = (count < MAX_BATTERY_SAMPLES) ? 0 : head;

  Serial.println("\n  #  | +min  | Voltage |  %");
  Serial.println("-----|-------|---------|-----");
  for (int i = 0; i < count; i++) {
    uint8_t idx = (start + i) % MAX_BATTERY_SAMPLES;
    Serial.printf("%4d | %5u | %.3fV  | %3d%%\n",
                  i + 1, history[idx].minutes, history[idx].voltage, history[idx].percent);
  }

  printVoltageChart(history, count, start);
  Serial.println("======================");
}

static void handleSerialCommand(const String &cmd) {
  if (cmd.equalsIgnoreCase("battery status")) {
    printBatteryStatus();
  }
}

void Bridge::begin() {
  // 1. Load saved slot
  _preferences.begin("usb-ble", true);
  _currentSlot = _preferences.getUChar("slot", 0);
  if (_currentSlot >= NUM_DEVICE_SLOTS)
    _currentSlot = 0;
  _preferences.end();

  Serial.printf("[Config] Starting on device slot %d\n", _currentSlot + 1);

  // 2. Persist any bonds NimBLE may have written in the previous session,
  //    then load the bonds for this slot. This prevents loadSlotBonds from
  //    clearing nimble_bond when ble_bond_X has not been saved yet.
  NVSUtils::saveSlotBonds(_currentSlot);
  NVSUtils::loadSlotBonds(_currentSlot);

  // 3. Init BLE
  const char *deviceNames[NUM_DEVICE_SLOTS] = {DEVICE_NAME_1, DEVICE_NAME_2,
                                               DEVICE_NAME_3};
  _bleManager.begin(_currentSlot, deviceNames[_currentSlot]);

  // 4. Init USB
  USBManager::setKeyboardCallback(onKeyboardReport);
  USBManager::begin();

  // 5. Configure battery ADC pin
  analogSetPinAttenuation(BATTERY_ADC_PIN, ADC_11db);
  analogReadResolution(12);

  // 6. Configure low-battery LED pin
  if (BATTERY_LOW_LED_PIN >= 0) {
    pinMode(BATTERY_LOW_LED_PIN, OUTPUT);
    digitalWrite(BATTERY_LOW_LED_PIN, LOW);
  }
}

void Bridge::loop() {
  // Track connection state changes and persist bonds so reconnect works after reboot
  static bool wasConnected = false;
  bool connected = _bleManager.isConnected();
  if (connected != wasConnected) {
    wasConnected = connected;
    NVSUtils::saveSlotBonds(_currentSlot);
    Serial.printf("[BLE] %s - bonds saved for slot %d\n",
                  connected ? "Connected" : "Disconnected", _currentSlot + 1);
  }

  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 5000) {
    lastStatus = millis();
    Serial.printf("[Status] Slot %d | BLE: %s\n", _currentSlot + 1,
                  connected ? "CONNECTED" : "waiting for pairing...");
  }

  static unsigned long lastBattery = 0;
  if (millis() - lastBattery > BATTERY_UPDATE_INTERVAL_MS) {
    lastBattery = millis();
    float voltage;
    uint8_t pct;
    readBattery(voltage, pct);
    _bleManager.setBatteryLevel(pct);
    Serial.printf("[Battery] Level: %d%%\n", pct);
    if (BATTERY_LOW_LED_PIN >= 0) {
      digitalWrite(BATTERY_LOW_LED_PIN, pct <= BATTERY_LOW_THRESHOLD ? HIGH : LOW);
    }
  }

  static unsigned long lastHistorySave = 0;
  if (millis() - lastHistorySave > BATTERY_HISTORY_INTERVAL_MS) {
    lastHistorySave = millis();
    float voltage;
    uint8_t pct;
    readBattery(voltage, pct);
    uint32_t minutes = millis() / 60000UL;
    NVSUtils::saveBatterySample(voltage, pct, minutes);
    Serial.printf("[Battery] History saved: %.3fV %d%% at +%u min\n", voltage, pct, minutes);
  }

  static String serialCmd;
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      serialCmd.trim();
      if (serialCmd.length() > 0) {
        handleSerialCommand(serialCmd);
      }
      serialCmd = "";
    } else {
      serialCmd += c;
    }
  }
}

void Bridge::switchToSlot(uint8_t slot) {
  if (slot >= NUM_DEVICE_SLOTS)
    return;

  if (slot == _currentSlot) {
    Serial.printf("[BLE] Already on slot %d\n", slot + 1);
    if (LED_FEEDBACK_PIN >= 0) {
      for (int i = 0; i <= slot; i++) {
        digitalWrite(LED_FEEDBACK_PIN, HIGH);
        delay(150);
        digitalWrite(LED_FEEDBACK_PIN, LOW);
        delay(150);
      }
    }
    return;
  }

  Serial.printf("[BLE] Switching from slot %d to slot %d\n", _currentSlot + 1,
                slot + 1);

  // 1. Save current BLE bonds
  NVSUtils::saveSlotBonds(_currentSlot);

  // 2. Save new slot index
  _preferences.begin("usb-ble", false);
  _preferences.putUChar("slot", slot);
  _preferences.end();

  // 3. LED feedback
  if (LED_FEEDBACK_PIN >= 0) {
    for (int i = 0; i <= slot; i++) {
      digitalWrite(LED_FEEDBACK_PIN, HIGH);
      delay(150);
      digitalWrite(LED_FEEDBACK_PIN, LOW);
      delay(150);
    }
  }

  // 4. Cleanup USB
  usb_host_device_free_all();

  Serial.println("[System] Restarting to apply new slot settings...");
  delay(500);
  ESP.restart();
}

void Bridge::onKeyboardReport(const uint8_t *data, size_t length) {
  if (length < sizeof(hid_keyboard_input_report_boot_t))
    return;

  hid_keyboard_input_report_boot_t *kb_report =
      (hid_keyboard_input_report_boot_t *)data;

  // Check for device switching combo
  if (checkDeviceSwitchCombo(kb_report->key)) {
    return;
  }

  // Debug output
  Serial.printf("[KB] mod:0x%02X keys:[%02X %02X %02X %02X %02X %02X]\n",
                kb_report->modifier.val, kb_report->key[0], kb_report->key[1],
                kb_report->key[2], kb_report->key[3], kb_report->key[4],
                kb_report->key[5]);

  // Forward to BLE
  _bleManager.sendKeyboardReport(kb_report->key, kb_report->modifier.val);
}

bool Bridge::checkDeviceSwitchCombo(const uint8_t *keys) {
  if (!ENABLE_DEVICE_SWITCHING)
    return false;

  bool hasScrollLock = false;
  uint8_t numberKey = 0;

  for (int i = 0; i < 6; i++) {
    if (keys[i] == HID_KEY_SCROLL_LOCK)
      hasScrollLock = true;
    if (keys[i] >= HID_KEY_1 && keys[i] <= HID_KEY_3) {
      numberKey = keys[i] - HID_KEY_1 + 1;
    }
  }

  if (hasScrollLock && numberKey > 0 && numberKey <= NUM_DEVICE_SLOTS) {
    Serial.printf("[Switch] Scroll Lock + %d detected\n", numberKey);
    switchToSlot(numberKey - 1);
    return true;
  }

  return false;
}
