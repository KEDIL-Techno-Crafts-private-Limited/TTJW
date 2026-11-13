/*
 * TTJW keypad and menu state machine.
 *
 * HOME:
 *   S1 = increase setpoint
 *   S3 = decrease setpoint
 *   S2 short press = open ADJ/SERV selection (starts at ADJ SET)
 *   S2 long press (3 s) = return HOME from any UI state
 *   S4 = no action
 *
 * Menus:
 *   S1/S3 = toggle or navigate
 *   S4 = select / enter edit / confirm
 */
#include "main.h"
#include "ttjw_config.h"
#include "ttjw_state.h"
#include "keypad.h"

static uint8_t s_rawKey = 0U;
static uint8_t s_stableKey = 0U;
static uint32_t s_nextScanTick = 0U;
static uint32_t s_rawChangeTick = 0U;
static uint32_t s_keyPressTick = 0U;
static uint32_t s_nextRepeatTick = 0U;
static uint8_t s_s2LongPressHandled = 0U;

static uint8_t Keypad_Scan(void);
static void Keypad_HandleShortPress(uint8_t key);
static void Keypad_HandleS2ShortPress(void);
static void Keypad_HandleRepeat(uint8_t key);
static void Keypad_HandleS1(void);
static void Keypad_HandleS3(void);
static void Keypad_HandleS4(void);
static void Keypad_GoHome(void);
static void Keypad_AdjustSetpoint(float direction);
static void Keypad_AdjustSelectedValue(float direction);
static void Keypad_MoveAdjParameter(int8_t direction);
static void Keypad_MoveServiceParameter(void);

static float DisplayDeltaToCelsius(float displayDelta)
{
    return (g_temperatureUnit == TEMP_UNIT_FAHRENHEIT) ?
           (displayDelta / 1.8f) : displayDelta;
}

void Keypad_Init(void)
{
    HAL_GPIO_WritePin(KEY_L1_GPIO_Port, KEY_L1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEY_L2_GPIO_Port, KEY_L2_Pin, GPIO_PIN_SET);

    s_rawKey = 0U;
    s_stableKey = 0U;
    s_nextScanTick = HAL_GetTick();
    s_rawChangeTick = HAL_GetTick();
    s_keyPressTick = 0U;
    s_nextRepeatTick = 0U;
    s_s2LongPressHandled = 0U;
}

/* Matrix:
 * L1/R1 = S1, L2/R1 = S2, L1/R2 = S3, L2/R2 = S4.
 */
static uint8_t Keypad_Scan(void)
{
    GPIO_PinState row1;
    GPIO_PinState row2;
    uint8_t key = 0U;

    HAL_GPIO_WritePin(KEY_L1_GPIO_Port, KEY_L1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(KEY_L2_GPIO_Port, KEY_L2_Pin, GPIO_PIN_SET);
    HAL_Delay(1U);

    row1 = HAL_GPIO_ReadPin(KEY_R1_GPIO_Port, KEY_R1_Pin);
    row2 = HAL_GPIO_ReadPin(KEY_R2_GPIO_Port, KEY_R2_Pin);

    if (row1 == GPIO_PIN_RESET)
    {
        key = KEY_S1;
    }
    else if (row2 == GPIO_PIN_RESET)
    {
        key = KEY_S3;
    }

    if (key == 0U)
    {
        HAL_GPIO_WritePin(KEY_L1_GPIO_Port, KEY_L1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(KEY_L2_GPIO_Port, KEY_L2_Pin, GPIO_PIN_RESET);
        HAL_Delay(1U);

        row1 = HAL_GPIO_ReadPin(KEY_R1_GPIO_Port, KEY_R1_Pin);
        row2 = HAL_GPIO_ReadPin(KEY_R2_GPIO_Port, KEY_R2_Pin);

        if (row1 == GPIO_PIN_RESET)
        {
            key = KEY_S2;
        }
        else if (row2 == GPIO_PIN_RESET)
        {
            key = KEY_S4;
        }
    }

    HAL_GPIO_WritePin(KEY_L1_GPIO_Port, KEY_L1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEY_L2_GPIO_Port, KEY_L2_Pin, GPIO_PIN_SET);

    return key;
}

void Keypad_Process(void)
{
    uint32_t now = HAL_GetTick();
    uint8_t scannedKey;

    if ((int32_t)(now - s_nextScanTick) < 0)
    {
        return;
    }

    s_nextScanTick = now + KEYPAD_SCAN_PERIOD_MS;
    scannedKey = Keypad_Scan();

    if (scannedKey != s_rawKey)
    {
        s_rawKey = scannedKey;
        s_rawChangeTick = HAL_GetTick();
        return;
    }

    now = HAL_GetTick();

    /* Debounced transition. */
    if ((s_stableKey != s_rawKey) &&
        ((uint32_t)(now - s_rawChangeTick) >= KEYPAD_DEBOUNCE_MS))
    {
        uint8_t previouslyStableKey = s_stableKey;
        s_stableKey = s_rawKey;

        if (s_stableKey == 0U)
        {
            /* S2 short press is the context-sensitive BACK/MODE key.
             * Long S2 hold is handled below and must not also trigger this.
             */
            if ((previouslyStableKey == KEY_S2) &&
                (s_s2LongPressHandled == 0U) &&
                ((uint32_t)(now - s_keyPressTick) < KEYPAD_S2_LONG_PRESS_MS))
            {
                Keypad_HandleS2ShortPress();
            }

            s_s2LongPressHandled = 0U;
        }
        else
        {
            s_keyPressTick = now;
            s_s2LongPressHandled = 0U;
            s_nextRepeatTick = now + KEYPAD_HOLD_INITIAL_MS;

            if (s_stableKey != KEY_S2)
            {
                Keypad_HandleShortPress(s_stableKey);
            }
        }
    }

    /* S2 is a dedicated global 3-second HOME/EXIT key. */
    if ((s_stableKey == KEY_S2) &&
        (s_s2LongPressHandled == 0U) &&
        ((uint32_t)(now - s_keyPressTick) >= KEYPAD_S2_LONG_PRESS_MS))
    {
        s_s2LongPressHandled = 1U;
        Keypad_GoHome();
    }

    /* Auto-repeat only for increment/decrement keys. */
    if (((s_stableKey == KEY_S1) || (s_stableKey == KEY_S3)) &&
        (s_stableKey == s_rawKey) &&
        ((int32_t)(now - s_nextRepeatTick) >= 0))
    {
        Keypad_HandleRepeat(s_stableKey);
        s_nextRepeatTick = now + KEYPAD_HOLD_REPEAT_MS;
    }
}

static void Keypad_HandleShortPress(uint8_t key)
{
    switch (key)
    {
        case KEY_S1:
            Keypad_HandleS1();
            break;

        case KEY_S3:
            Keypad_HandleS3();
            break;

        case KEY_S4:
            Keypad_HandleS4();
            break;

        case KEY_S2:
        default:
            /* S2 short press intentionally has no action. */
            break;
    }
}

static void Keypad_HandleS2ShortPress(void)
{
    switch (g_uiState)
    {
        case UI_HOME:
            /* HOME -> mode selection, initially ADJ SET. */
            g_uiState = UI_MENU_ADJ;
            break;

        case UI_MENU_ADJ:
        case UI_MENU_SERV:
            /* Either mode-selection option -> HOME. */
            g_uiState = UI_HOME;
            break;

        case UI_ADJ_UNIT:
        case UI_ADJ_UNIT_EDIT:
        case UI_ADJ_ALARM_TYPE:
        case UI_ADJ_ALARM_TYPE_EDIT:
        case UI_ADJ_ALARM_VALUE:
        case UI_ADJ_ALARM_VALUE_EDIT:
        case UI_ADJ_SFT:
        case UI_ADJ_SFT_EDIT:
        case UI_SERV_AUTO_CAL:
        case UI_SERV_AUTO_CAL_SELECTED:
        case UI_SERV_RESET:
        case UI_SERV_RESET_SELECTED:
            /* Parameter/service screen -> mode selection. */
            g_uiState = UI_MENU_ADJ;
            break;

        default:
            g_uiState = UI_HOME;
            break;
    }

    g_displayNeedsRefresh = 1U;
}

static void Keypad_HandleRepeat(uint8_t key)
{
    if ((g_uiState == UI_ADJ_UNIT_EDIT) ||
        (g_uiState == UI_SERV_AUTO_CAL_SELECTED) ||
        (g_uiState == UI_SERV_RESET_SELECTED))
    {
        return;
    }

    if (key == KEY_S1)
    {
        Keypad_HandleS1();
    }
    else if (key == KEY_S3)
    {
        Keypad_HandleS3();
    }
}

static void Keypad_GoHome(void)
{
    g_uiState = UI_HOME;
    g_displayNeedsRefresh = 1U;
}

static void Keypad_AdjustSetpoint(float direction)
{
    float step;
    float maximumC;

    step = (g_temperatureUnit == TEMP_UNIT_FAHRENHEIT) ?
           (1.0f / 1.8f) : SV_STEP;

    g_setValue += direction * step;

    if (g_temperatureUnit == TEMP_UNIT_FAHRENHEIT)
    {
        maximumC = (99.99f - 32.0f) / 1.8f;
    }
    else
    {
        maximumC = SV_MAX_VALUE;
    }

    if (g_setValue > maximumC)
    {
        g_setValue = maximumC;
    }
    if (g_setValue < SV_MIN_VALUE)
    {
        g_setValue = SV_MIN_VALUE;
    }

    g_displayNeedsRefresh = 1U;
}

static void Keypad_AdjustSelectedValue(float direction)
{
    float step;
    float maxC;
    float minC;

    switch (g_uiState)
    {
        case UI_ADJ_UNIT_EDIT:
            /* Only two unit choices exist; either direction toggles. */
            g_temperatureUnit = (g_temperatureUnit == TEMP_UNIT_CELSIUS) ?
                                TEMP_UNIT_FAHRENHEIT : TEMP_UNIT_CELSIUS;
            break;

        case UI_ADJ_ALARM_TYPE_EDIT:
            /* S3 advances: UP -> LO -> P-L -> OFF -> UP.
             * S1 moves in the reverse direction.
             */
            if (direction < 0.0f) /* S3: next = UP -> LO -> P-L -> OFF */
            {
                if (g_alarmType == 1U) g_alarmType = 2U;
                else if (g_alarmType == 2U) g_alarmType = 3U;
                else if (g_alarmType == 3U) g_alarmType = 0U;
                else g_alarmType = 1U;
            }
            else /* S1: previous */
            {
                if (g_alarmType == 1U) g_alarmType = 0U;
                else if (g_alarmType == 0U) g_alarmType = 3U;
                else if (g_alarmType == 3U) g_alarmType = 2U;
                else g_alarmType = 1U;
            }
            break;

        case UI_ADJ_ALARM_VALUE_EDIT:
            step = DisplayDeltaToCelsius(TTJW_ALARM_STEP);
            g_alarmValue += direction * step;

            maxC = (g_temperatureUnit == TEMP_UNIT_FAHRENHEIT) ?
                   (99.99f / 1.8f) : TTJW_ALARM_MAX_C;
            if (g_alarmValue < 0.0f) g_alarmValue = 0.0f;
            if (g_alarmValue > maxC) g_alarmValue = maxC;
            break;

        case UI_ADJ_SFT_EDIT:
            step = DisplayDeltaToCelsius(TTJW_SFT_STEP);
            g_errorAdjustment += direction * step;

            if (g_temperatureUnit == TEMP_UNIT_FAHRENHEIT)
            {
                maxC = 99.99f / 1.8f;
                minC = -9.99f / 1.8f;
            }
            else
            {
                maxC = ERROR_MAX_VALUE;
                minC = ERROR_MIN_VALUE;
            }

            if (g_errorAdjustment > maxC) g_errorAdjustment = maxC;
            if (g_errorAdjustment < minC) g_errorAdjustment = minC;
            break;

        default:
            break;
    }

    g_displayNeedsRefresh = 1U;
}

static void Keypad_MoveAdjParameter(int8_t direction)
{
    switch (g_uiState)
    {
        case UI_ADJ_UNIT:
            g_uiState = (direction > 0) ?
                        UI_ADJ_ALARM_TYPE : UI_ADJ_SFT;
            break;

        case UI_ADJ_ALARM_TYPE:
            g_uiState = (direction > 0) ?
                        UI_ADJ_ALARM_VALUE : UI_ADJ_UNIT;
            break;

        case UI_ADJ_ALARM_VALUE:
            g_uiState = (direction > 0) ?
                        UI_ADJ_SFT : UI_ADJ_ALARM_TYPE;
            break;

        case UI_ADJ_SFT:
        default:
            g_uiState = (direction > 0) ?
                        UI_ADJ_UNIT : UI_ADJ_ALARM_VALUE;
            break;
    }

    g_displayNeedsRefresh = 1U;
}

static void Keypad_MoveServiceParameter(void)
{
    if ((g_uiState == UI_SERV_AUTO_CAL) ||
        (g_uiState == UI_SERV_AUTO_CAL_SELECTED))
    {
        g_uiState = UI_SERV_RESET;
    }
    else
    {
        g_uiState = UI_SERV_AUTO_CAL;
    }

    g_displayNeedsRefresh = 1U;
}

static void Keypad_HandleS1(void)
{
    switch (g_uiState)
    {
        case UI_HOME:
            /* Preserve the existing HOME setpoint increment action. */
            Keypad_AdjustSetpoint(1.0f);
            break;

        case UI_MENU_ADJ:
            g_uiState = UI_MENU_SERV;
            g_displayNeedsRefresh = 1U;
            break;

        case UI_MENU_SERV:
            g_uiState = UI_MENU_ADJ;
            g_displayNeedsRefresh = 1U;
            break;

        case UI_ADJ_UNIT:
        case UI_ADJ_ALARM_TYPE:
        case UI_ADJ_ALARM_VALUE:
        case UI_ADJ_SFT:
            Keypad_MoveAdjParameter(-1);
            break;

        case UI_ADJ_UNIT_EDIT:
        case UI_ADJ_ALARM_TYPE_EDIT:
        case UI_ADJ_ALARM_VALUE_EDIT:
        case UI_ADJ_SFT_EDIT:
            Keypad_AdjustSelectedValue(1.0f);
            break;

        case UI_SERV_AUTO_CAL:
        case UI_SERV_RESET:
            Keypad_MoveServiceParameter();
            break;

        case UI_SERV_AUTO_CAL_SELECTED:
        case UI_SERV_RESET_SELECTED:
            /* Frontend-only selected screen: S1/S3 do not execute an action. */
            break;

        default:
            break;
    }
}

static void Keypad_HandleS3(void)
{
    switch (g_uiState)
    {
        case UI_HOME:
            /* On HOME, S3 always decreases the setpoint.
             * Menu entry is reserved for S2 short press.
             */
            Keypad_AdjustSetpoint(-1.0f);
            break;

        case UI_MENU_ADJ:
            g_uiState = UI_MENU_SERV;
            g_displayNeedsRefresh = 1U;
            break;

        case UI_MENU_SERV:
            g_uiState = UI_MENU_ADJ;
            g_displayNeedsRefresh = 1U;
            break;

        case UI_ADJ_UNIT:
        case UI_ADJ_ALARM_TYPE:
        case UI_ADJ_ALARM_VALUE:
        case UI_ADJ_SFT:
            Keypad_MoveAdjParameter(1);
            break;

        case UI_ADJ_UNIT_EDIT:
        case UI_ADJ_ALARM_TYPE_EDIT:
        case UI_ADJ_ALARM_VALUE_EDIT:
        case UI_ADJ_SFT_EDIT:
            Keypad_AdjustSelectedValue(-1.0f);
            break;

        case UI_SERV_AUTO_CAL:
        case UI_SERV_RESET:
            Keypad_MoveServiceParameter();
            break;

        case UI_SERV_AUTO_CAL_SELECTED:
        case UI_SERV_RESET_SELECTED:
            /* Frontend-only selected screen: S1/S3 do not execute an action. */
            break;

        default:
            break;
    }
}

static void Keypad_HandleS4(void)
{
    switch (g_uiState)
    {
        case UI_MENU_ADJ:
            g_uiState = UI_ADJ_UNIT;
            break;

        case UI_MENU_SERV:
            g_uiState = UI_SERV_AUTO_CAL;
            break;

        case UI_ADJ_UNIT:
            g_uiState = UI_ADJ_UNIT_EDIT;
            break;
        case UI_ADJ_UNIT_EDIT:
            g_uiState = UI_ADJ_UNIT;
            break;

        case UI_ADJ_ALARM_TYPE:
            g_uiState = UI_ADJ_ALARM_TYPE_EDIT;
            break;
        case UI_ADJ_ALARM_TYPE_EDIT:
            g_uiState = UI_ADJ_ALARM_TYPE;
            break;

        case UI_ADJ_ALARM_VALUE:
            g_uiState = UI_ADJ_ALARM_VALUE_EDIT;
            break;
        case UI_ADJ_ALARM_VALUE_EDIT:
            g_uiState = UI_ADJ_ALARM_VALUE;
            break;

        case UI_ADJ_SFT:
            g_uiState = UI_ADJ_SFT_EDIT;
            break;
        case UI_ADJ_SFT_EDIT:
            g_uiState = UI_ADJ_SFT;
            break;

        case UI_SERV_AUTO_CAL:
            /* Frontend selection only; no calibration action is triggered. */
            g_uiState = UI_SERV_AUTO_CAL_SELECTED;
            break;
        case UI_SERV_AUTO_CAL_SELECTED:
            g_uiState = UI_SERV_AUTO_CAL;
            break;

        case UI_SERV_RESET:
            /* Frontend selection only; no settings are reset. */
            g_uiState = UI_SERV_RESET_SELECTED;
            break;
        case UI_SERV_RESET_SELECTED:
            g_uiState = UI_SERV_RESET;
            break;

        case UI_HOME:
        default:
            break;
    }

    g_displayNeedsRefresh = 1U;
}
