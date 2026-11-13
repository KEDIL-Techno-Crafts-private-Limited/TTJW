#include "ttjw_state.h"

/* Process values and controller settings are stored in Celsius. */
float g_setValue = 37.0f;
float g_errorAdjustment = 0.0f;
float g_pidPValue = 30.0f;
float g_pidIValue = 0.0f;
float g_pidDValue = 0.0f;

float g_latestTemperature = 0.0f;
float g_measuredTemperature = 0.0f;
float g_alarmValue = 0.9f;

uint8_t g_alarmType = 1U; /* UP */
uint8_t g_heaterDutyPercent = 0U;
TemperatureUnit_t g_temperatureUnit = TEMP_UNIT_CELSIUS;

TTJW_UiState_t g_uiState = UI_HOME;
volatile uint8_t g_displayNeedsRefresh = 1U;
volatile uint8_t g_errorDisplayActive = 0U;
volatile uint8_t g_sensorValid = 0U;
volatile uint8_t g_alarmActive = 0U;
