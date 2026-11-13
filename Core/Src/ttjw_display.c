/*
 * TTJW two-module TM1637 display rendering.
 * PV is the left module; SV / menu value is the right module.
 */
#include "main.h"
#include "tm1637.h"
#include "ttjw_state.h"
#include "ttjw_display.h"
#include <stdint.h>

static TM1637_Handle_t s_pv = {
    PV_TM_CLK_GPIO_Port, PV_TM_CLK_Pin,
    PV_TM_DIO_GPIO_Port, PV_TM_DIO_Pin
};

static TM1637_Handle_t s_sv = {
    SV_TM_CLK_GPIO_Port, SV_TM_CLK_Pin,
    SV_TM_DIO_GPIO_Port, SV_TM_DIO_Pin
};

static float ToDisplayTemperature(float celsius)
{
    if (g_temperatureUnit == TEMP_UNIT_FAHRENHEIT)
    {
        return (celsius * 1.8f) + 32.0f;
    }

    return celsius;
}

static float ToDisplayDelta(float celsiusDelta)
{
    if (g_temperatureUnit == TEMP_UNIT_FAHRENHEIT)
    {
        return celsiusDelta * 1.8f;
    }

    return celsiusDelta;
}

static int32_t ToHundredths(float value)
{
    float scaled = value * 100.0f;

    if (scaled >= 0.0f)
    {
        return (int32_t)(scaled + 0.5f);
    }

    return (int32_t)(scaled - 0.5f);
}

static void DisplayTextPair(const char pvText[4],
                            const char svText[4])
{
    TM1637_DisplayTextWithColon(&s_pv, pvText, 0U);
    TM1637_DisplayTextWithColon(&s_sv, svText, 0U);
}

static void DisplayParameterValue(float value)
{
    TM1637_DisplayFixed2WithColon(&s_sv,
                                  ToHundredths(ToDisplayDelta(value)),
                                  1U);
}

void TTJW_DisplayInit(void)
{
    TM1637_Init(&s_pv);
    TM1637_Init(&s_sv);

    TM1637_SetBrightness(&s_pv, 7U);
    TM1637_SetBrightness(&s_sv, 7U);
}

void TTJW_DisplayUpdate(void)
{
    /* A sensor fault remains visible regardless of the active menu. */
    if (g_errorDisplayActive != 0U)
    {
        DisplayTextPair("Err ", "    ");
        g_displayNeedsRefresh = 0U;
        return;
    }

    switch (g_uiState)
    {
        case UI_HOME:
            TM1637_DisplayFixed2WithColon(
                &s_pv,
                ToHundredths(ToDisplayTemperature(g_latestTemperature)),
                1U);

            TM1637_DisplayFixed2WithColon(
                &s_sv,
                ToHundredths(ToDisplayTemperature(g_setValue)),
                1U);
            break;

        /* Top-level selection: ADJ SET / SERV SET. */
        case UI_MENU_ADJ:
            DisplayTextPair(" ADJ", " SET");
            break;

        case UI_MENU_SERV:
            DisplayTextPair("SERV", " SET");
            break;

        /* ADJ parameter browsing and editing use the same parameter label.
         * S4 changes the internal state from browse to edit.
         */
        case UI_ADJ_UNIT:
        case UI_ADJ_UNIT_EDIT:
            TM1637_DisplayTextWithColon(&s_pv, "UNIT", 0U);
            TM1637_DisplayTextWithColon(
                &s_sv,
                (g_temperatureUnit == TEMP_UNIT_CELSIUS) ? "   C" : "   F",
                0U);
            break;

        case UI_ADJ_ALARM_TYPE:
        case UI_ADJ_ALARM_TYPE_EDIT:
            TM1637_DisplayTextWithColon(&s_pv, "AL-T", 0U);
            switch (g_alarmType)
            {
                case 1U:
                    TM1637_DisplayTextWithColon(&s_sv, "UP  ", 0U);
                    break;
                case 2U:
                    TM1637_DisplayTextWithColon(&s_sv, "LO  ", 0U);
                    break;
                case 3U:
                    TM1637_DisplayTextWithColon(&s_sv, "P-L ", 0U);
                    break;
                case 0U:
                default:
                    TM1637_DisplayTextWithColon(&s_sv, "OFF ", 0U);
                    break;
            }
            break;

        case UI_ADJ_ALARM_VALUE:
        case UI_ADJ_ALARM_VALUE_EDIT:
            TM1637_DisplayTextWithColon(&s_pv, "AL-V", 0U);
            DisplayParameterValue(g_alarmValue);
            break;

        case UI_ADJ_SFT:
        case UI_ADJ_SFT_EDIT:
            TM1637_DisplayTextWithColon(&s_pv, "SFT ", 0U);
            DisplayParameterValue(g_errorAdjustment);
            break;

        /* Service choices. ATO-CLB means Auto Calibration.
         * RST-SET means Reset Settings. These are frontend-only selections.
         */
        case UI_SERV_AUTO_CAL:
        case UI_SERV_AUTO_CAL_SELECTED:
            DisplayTextPair(" ATO", " CLB");
            break;

        case UI_SERV_RESET:
        case UI_SERV_RESET_SELECTED:
            DisplayTextPair(" RST", " SET");
            break;

        default:
            g_uiState = UI_HOME;
            TM1637_DisplayFixed2WithColon(
                &s_pv,
                ToHundredths(ToDisplayTemperature(g_latestTemperature)),
                1U);
            TM1637_DisplayFixed2WithColon(
                &s_sv,
                ToHundredths(ToDisplayTemperature(g_setValue)),
                1U);
            break;
    }

    g_displayNeedsRefresh = 0U;
}
