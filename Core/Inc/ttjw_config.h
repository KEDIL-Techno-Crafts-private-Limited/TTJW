/* TTJW configuration: tune only after checking hardware timing and safety requirements. */
#ifndef TTJW_CONFIG_H
#define TTJW_CONFIG_H


/* ADS1220 register configuration */
#define ADS1220_CMD_RESET       0x06U
#define ADS1220_CMD_START       0x08U
#define ADS1220_CMD_WREG_4REG   0x43U
#define ADS1220_REG0            0x66U
#define ADS1220_REG1            0x04U
#define ADS1220_REG2            0x55U
#define ADS1220_REG3            0x70U

/* PT100 analogue front-end constants */
#define RREF_OHMS               1624.0f
#define RTD_GAIN                8.0f
#define ADC_FULL_SCALE          8388608.0f
#define PT100_R0                100.0f
#define PT100_A                 0.0039083f
#define PT100_B                -0.0000005775f

/* Setpoint and adjustment ranges */
#define SV_MIN_VALUE              0.0f
#define SV_MAX_VALUE             99.99f
#define SV_STEP                   1.0f
#define ERROR_MIN_VALUE          (-9.99f)
#define ERROR_MAX_VALUE            99.99f
#define ERROR_STEP                0.1f
#define PID_VALUE_MIN             0.0f
#define PID_VALUE_MAX          9999.0f

/* Keypad scan and repeat timing */
#define KEY_S1                    1U
#define KEY_S2                    2U
#define KEY_S3                    3U
#define KEY_S4                    4U
#define KEYPAD_SCAN_PERIOD_MS     5U
#define KEYPAD_DEBOUNCE_MS       30U
#define KEYPAD_HOLD_INITIAL_MS  500U
#define KEYPAD_HOLD_REPEAT_MS   150U
#define KEYPAD_S2_LONG_PRESS_MS 3000U

/* ADC fault timeout */
#define ADS_DRDY_TIMEOUT_MS    2000U

/* TIM1 CH1: 10 kHz counter, ARR 19999 => 2-second PWM time window. */
#define HEATER_PWM_PERIOD      19999U
#define HEATER_WINDOW_MS       2000U
#define HEATER_DUTY_STEP       10U
#define HEATER_MAX_DUTY        100U
#define HEATER_KP_PERCENT_PER_C 30.0f

/* Heater output is deliberately disabled until bench validation is completed.
 * Set to 1 only after checking the power stage, independent thermal cutoff,
 * sensor fault response and maximum-temperature protection on hardware.
 */
#define TTJW_HEATER_OUTPUT_ENABLE 0U

/* Set to 1 only in this dedicated bench-test project. PA8 drives a logic-level
 * measurement point; heater power must remain physically disconnected. */
#define TTJW_PID_PWM_TEST_MODE 1U

/* ADC timeout and basic electrical plausibility window for PT100 wiring.
 * This window is for detecting open/short-like readings, not a certified
 * operating-temperature safety limit.
 */
#define TTJW_PT100_RESISTANCE_MIN_OHMS 50.0f
#define TTJW_PT100_RESISTANCE_MAX_OHMS 300.0f

#define TTJW_ALARM_STEP 0.1f
#define TTJW_ALARM_MAX_C 99.9f
#define TTJW_SFT_STEP 0.1f

#endif /* TTJW_CONFIG_H */
