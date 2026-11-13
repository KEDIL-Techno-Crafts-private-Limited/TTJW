#ifndef TTJW_STATE_H
#define TTJW_STATE_H

#include <stdint.h>

typedef enum
{
    UI_HOME = 0,

    /* Top-level menu choices */
    UI_MENU_ADJ,
    UI_MENU_SERV,

    /* ADJ parameter browsing and editing */
    UI_ADJ_UNIT,
    UI_ADJ_UNIT_EDIT,
    UI_ADJ_ALARM_TYPE,
    UI_ADJ_ALARM_TYPE_EDIT,
    UI_ADJ_ALARM_VALUE,
    UI_ADJ_ALARM_VALUE_EDIT,
    UI_ADJ_SFT,
    UI_ADJ_SFT_EDIT,

    /* SERV choices and frontend-only selected states */
    UI_SERV_AUTO_CAL,
    UI_SERV_AUTO_CAL_SELECTED,
    UI_SERV_RESET,
    UI_SERV_RESET_SELECTED
} TTJW_UiState_t;

typedef enum
{
    TEMP_UNIT_CELSIUS = 0,
    TEMP_UNIT_FAHRENHEIT
} TemperatureUnit_t;

extern float g_setValue;
extern float g_errorAdjustment;
extern float g_pidPValue;
extern float g_pidIValue;
extern float g_pidDValue;
extern float g_latestTemperature;       /* Corrected PV, Celsius internally. */
extern float g_measuredTemperature;     /* Raw measured PV, Celsius. */
extern float g_alarmValue;              /* Deviation, Celsius internally. */

extern uint8_t g_alarmType;              /* 0=OFF, 1=UP, 2=LO, 3=P-L. */
extern TemperatureUnit_t g_temperatureUnit;
extern uint8_t g_heaterDutyPercent;
extern TTJW_UiState_t g_uiState;
extern volatile uint8_t g_displayNeedsRefresh;
extern volatile uint8_t g_errorDisplayActive;
extern volatile uint8_t g_sensorValid;
extern volatile uint8_t g_alarmActive;

#endif
