/**
 * @file Config.h
 * @brief Global configuration constants for the USB-to-BLE Bridge.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// CONFIGURATION - Edit these to customize behavior
// ============================================================================

/** @brief Device names for each slot (will show up in Bluetooth settings) */
#define DEVICE_NAME_1 "Keyboard pipe 1"
#define DEVICE_NAME_2 "Keyboard pipe 2"
#define DEVICE_NAME_3 "Keyboard pipe 3"

/** @brief Manufacturer name reported over BLE */
#define DEVICE_MANUFACTURER "OS Designs"

/** @brief Initial battery level reported over BLE before first ADC reading (0-100) */
#define BATTERY_LEVEL 100

// ============================================================================
// BATTERY MONITORING - Resistor voltage divider on ADC pin
// ============================================================================

/** @brief GPIO pin connected to the midpoint of the battery voltage divider */
#define BATTERY_ADC_PIN 1

/** @brief Resistor from battery positive terminal to ADC pin (ohms) - 1M */
#define BATTERY_R1 1000000.0f

/** @brief Resistor from ADC pin to GND (ohms) - 1M */
#define BATTERY_R2 1000000.0f

/** @brief How often to read battery voltage and update BLE (milliseconds) */
#define BATTERY_UPDATE_INTERVAL_MS 30000UL

/** @brief Number of available device slots (maximum 3 recommended) */
#define NUM_DEVICE_SLOTS 3

/** @brief Key combo for device switching: Scroll Lock + number key */
#define ENABLE_DEVICE_SWITCHING true

/**
 * @brief LED feedback pin (blinks to show current slot)
 * Set to -1 to disable, or use your board's built-in LED pin
 */
#define LED_FEEDBACK_PIN 2

#endif // CONFIG_H
