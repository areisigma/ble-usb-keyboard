/**
 * @file NVSUtils.h
 * @brief Utilities for Non-Volatile Storage (NVS) management, specifically BLE
 * bonds.
 */

#ifndef NVS_UTILS_H
#define NVS_UTILS_H

#include <stdint.h>

/** @brief Maximum number of battery history samples stored in flash (FIFO circular buffer). */
#define MAX_BATTERY_SAMPLES 100

/** @brief One battery measurement stored to flash. Packed to avoid padding waste in NVS blob. */
struct BatterySample {
  float    voltage;  ///< Battery voltage in volts
  uint32_t minutes;  ///< Device uptime in minutes when sample was taken
  uint8_t  percent;  ///< Battery percentage (0-100)
} __attribute__((packed));

class NVSUtils {
public:
  /**
   * @brief Copies all keys from one NVS namespace to another.
   * @param src_ns Source namespace name.
   * @param dst_ns Destination namespace name.
   */
  static void copyNamespace(const char *src_ns, const char *dst_ns);

  /**
   * @brief Loads stored BLE bonds for a specific slot into the active BLE
   * namespace.
   * @param slot Slot index (0-based).
   */
  static void loadSlotBonds(uint8_t slot);

  /**
   * @brief Saves current active BLE bonds into the storage for a specific slot.
   * @param slot Slot index (0-based).
   */
  static void saveSlotBonds(uint8_t slot);

  /**
   * @brief Appends one battery sample to the circular flash history (max MAX_BATTERY_SAMPLES).
   * @param voltage  Battery voltage in volts.
   * @param percent  Battery percentage (0-100).
   * @param minutes  Device uptime in minutes.
   */
  static void saveBatterySample(float voltage, uint8_t percent, uint32_t minutes);

  /**
   * @brief Loads the battery history from flash.
   * @param out    Caller-supplied array of MAX_BATTERY_SAMPLES elements.
   * @param count  Number of valid samples written into out (0..MAX_BATTERY_SAMPLES).
   * @param head   Next-write index; oldest sample is at head when count==MAX_BATTERY_SAMPLES,
   *               otherwise oldest is at index 0.
   */
  static void loadBatteryHistory(BatterySample *out, uint8_t &count, uint8_t &head);
};

#endif // NVS_UTILS_H
