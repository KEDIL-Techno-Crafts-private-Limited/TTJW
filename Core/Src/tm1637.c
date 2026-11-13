
#include "tm1637.h"
#include <string.h>

/* TM1637 Commands */
#define TM1637_CMD_DATA       0x40U
#define TM1637_CMD_ADDR       0xC0U
#define TM1637_CMD_CTRL       0x80U

#define TM1637_DISPLAY_ON     0x08U
#define TM1637_DEFAULT_BRIGHTNESS 7U

/* Seven-segment digit patterns */
static const uint8_t segDigits[10] =
{
    0x3FU,  /* 0 */
    0x06U,  /* 1 */
    0x5BU,  /* 2 */
    0x4FU,  /* 3 */
    0x66U,  /* 4 */
    0x6DU,  /* 5 */
    0x7DU,  /* 6 */
    0x07U,  /* 7 */
    0x7FU,  /* 8 */
    0x6FU   /* 9 */
};

/* Brightness */
static uint8_t brightness = TM1637_DEFAULT_BRIGHTNESS;

/* =====================================================
   GPIO FUNCTIONS
   ===================================================== */

static void TM1637_Delay(void)
{
    for (volatile uint8_t i = 0; i < 30U; i++)
    {
        __NOP();
    }
}

static void TM1637_CLK(TM1637_Handle_t *dev,
                       GPIO_PinState state)
{
    HAL_GPIO_WritePin(dev->clkPort, dev->clkPin, state);
}

static void TM1637_DIO(TM1637_Handle_t *dev,
                       GPIO_PinState state)
{
    HAL_GPIO_WritePin(dev->dioPort, dev->dioPin, state);
}

static void TM1637_DIO_Output(TM1637_Handle_t *dev)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = dev->dioPin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(dev->dioPort, &GPIO_InitStruct);
}

static void TM1637_DIO_Input(TM1637_Handle_t *dev)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = dev->dioPin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(dev->dioPort, &GPIO_InitStruct);
}

/* =====================================================
   START / STOP CONDITIONS
   ===================================================== */

static void TM1637_Start(TM1637_Handle_t *dev)
{
    TM1637_DIO_Output(dev);

    TM1637_CLK(dev, GPIO_PIN_SET);
    TM1637_DIO(dev, GPIO_PIN_SET);

    TM1637_Delay();

    TM1637_DIO(dev, GPIO_PIN_RESET);

    TM1637_Delay();

    TM1637_CLK(dev, GPIO_PIN_RESET);

    TM1637_Delay();
}

static void TM1637_Stop(TM1637_Handle_t *dev)
{
    TM1637_DIO_Output(dev);

    TM1637_CLK(dev, GPIO_PIN_RESET);
    TM1637_DIO(dev, GPIO_PIN_RESET);

    TM1637_Delay();

    TM1637_CLK(dev, GPIO_PIN_SET);

    TM1637_Delay();

    TM1637_DIO(dev, GPIO_PIN_SET);

    TM1637_Delay();
}

/* =====================================================
   WRITE BYTE AND READ ACKNOWLEDGEMENT
   ===================================================== */

static uint8_t TM1637_WriteByte(TM1637_Handle_t *dev,
                                uint8_t data)
{
    uint8_t i;
    uint8_t ack;

    TM1637_DIO_Output(dev);

    for (i = 0; i < 8U; i++)
    {
        TM1637_CLK(dev, GPIO_PIN_RESET);

        if (data & 0x01U)
        {
            TM1637_DIO(dev, GPIO_PIN_SET);
        }
        else
        {
            TM1637_DIO(dev, GPIO_PIN_RESET);
        }

        TM1637_Delay();

        TM1637_CLK(dev, GPIO_PIN_SET);

        TM1637_Delay();

        data >>= 1U;
    }

    /* ACK cycle */
    TM1637_CLK(dev, GPIO_PIN_RESET);

    TM1637_DIO(dev, GPIO_PIN_SET);

    TM1637_DIO_Input(dev);

    TM1637_Delay();

    TM1637_CLK(dev, GPIO_PIN_SET);

    TM1637_Delay();

    ack = (HAL_GPIO_ReadPin(dev->dioPort,
                            dev->dioPin) == GPIO_PIN_RESET);

    TM1637_CLK(dev, GPIO_PIN_RESET);

    TM1637_DIO_Output(dev);

    return ack;
}

/* =====================================================
   INITIALIZATION
   ===================================================== */

void TM1637_Init(TM1637_Handle_t *dev)
{
    if (dev == NULL)
    {
        return;
    }

    TM1637_DIO_Output(dev);

    TM1637_CLK(dev, GPIO_PIN_SET);
    TM1637_DIO(dev, GPIO_PIN_SET);

    TM1637_SetBrightness(dev, TM1637_DEFAULT_BRIGHTNESS);

    /* Clear display */
    uint8_t blank[4] = {0U, 0U, 0U, 0U};

    TM1637_DisplayRaw(dev, blank);
}

/* =====================================================
   BRIGHTNESS CONTROL
   ===================================================== */

void TM1637_SetBrightness(TM1637_Handle_t *dev,
                          uint8_t level)
{
    if (dev == NULL)
    {
        return;
    }

    if (level > 7U)
    {
        level = 7U;
    }

    brightness = level;

    TM1637_Start(dev);

    (void)TM1637_WriteByte(dev,
                           TM1637_CMD_CTRL |
                           TM1637_DISPLAY_ON |
                           brightness);

    TM1637_Stop(dev);
}

/* =====================================================
   DISPLAY RAW DATA
   ===================================================== */

void TM1637_DisplayRaw(TM1637_Handle_t *dev,
                       const uint8_t digits[4])
{
    TM1637_DisplayRawWithColon(dev, digits, 1U);
}

void TM1637_DisplayRawWithColon(TM1637_Handle_t *dev,
                                const uint8_t digits[4],
                                uint8_t colonOn)
{
    uint8_t i;

    if ((dev == NULL) || (digits == NULL))
    {
        return;
    }

    /* Automatic address increment */
    TM1637_Start(dev);

    (void)TM1637_WriteByte(dev, TM1637_CMD_DATA);

    TM1637_Stop(dev);

    /* Write four digits */
    TM1637_Start(dev);

    (void)TM1637_WriteByte(dev, TM1637_CMD_ADDR);

    for (i = 0; i < 4U; i++)
    {
        /* On this module, the centre colon is controlled by bit 7
         * of digit index 1. It is selected per screen.
         */
        uint8_t pattern = digits[i];
        if ((i == 1U) && (colonOn != 0U))
        {
            pattern |= 0x80U;
        }
        else if (i == 1U)
        {
            pattern &= (uint8_t)~0x80U;
        }
        (void)TM1637_WriteByte(dev, pattern);
    }

    TM1637_Stop(dev);

    /* Display ON */
    TM1637_Start(dev);

    (void)TM1637_WriteByte(dev,
                           TM1637_CMD_CTRL |
                           TM1637_DISPLAY_ON |
                           brightness);

    TM1637_Stop(dev);
}

/* =====================================================
   DISPLAY NUMBER
   ===================================================== */

void TM1637_DisplayNumber(TM1637_Handle_t *dev,
                          int32_t value,
                          uint8_t decimalPosition)
{
    uint8_t out[4] = {0U, 0U, 0U, 0U};
    uint32_t number;
    uint8_t negative = 0U;
    int8_t i;

    if (dev == NULL)
    {
        return;
    }

    if (value < 0)
    {
        negative = 1U;

        /* Avoid overflow for INT32_MIN */
        number = (uint32_t)(-(int64_t)value);
    }
    else
    {
        number = (uint32_t)value;
    }

    /* Maximum displayable value */
    if ((!negative && number > 9999U) ||
        (negative && number > 999U))
    {
        uint8_t error[4] =
        {
            0x79U,  /* E */
            0x50U,  /* r */
            0x50U,  /* r */
            0x00U
        };

        TM1637_DisplayRaw(dev, error);
        return;
    }

    /* Extract digits from right to left */
    for (i = 3; i >= 0; i--)
    {
        if (negative && i == 0)
        {
            out[i] = 0x40U;  /* Minus sign */
        }
        else
        {
            out[i] = segDigits[number % 10U];
            number /= 10U;
        }
    }

    /* Apply decimal point */
    if (decimalPosition < 4U)
    {
        out[decimalPosition] |= 0x80U;
    }

    TM1637_DisplayRaw(dev, out);
}

/* =====================================================
   DISPLAY FIXED TWO DECIMAL PLACES
   ===================================================== */

void TM1637_DisplayFixed2(TM1637_Handle_t *dev, int32_t valueHundredths)
{
    TM1637_DisplayFixed2WithColon(dev, valueHundredths, 1U);
}

void TM1637_DisplayFixed2WithColon(TM1637_Handle_t *dev,
                                   int32_t valueHundredths,
                                   uint8_t colonOn)
{
    uint8_t out[4] = {0U, 0U, 0U, 0U};
    uint32_t magnitude;
    int8_t i;
    uint8_t negative = 0U;

    if (dev == NULL)
    {
        return;
    }

    if (valueHundredths < 0)
    {
        negative = 1U;
        magnitude = (uint32_t)(-(int64_t)valueHundredths);
    }
    else
    {
        magnitude = (uint32_t)valueHundredths;
    }

    /* Four positions are available: XX.XX. A negative value uses
       the first position for the minus sign, so its magnitude is
       limited to three digits. */
    if (((negative == 0U) && (magnitude > 9999U)) ||
        ((negative != 0U) && (magnitude > 999U)))
    {
        const uint8_t error[4] = {0x79U, 0x50U, 0x50U, 0x00U};
        TM1637_DisplayRawWithColon(dev, error, colonOn);
        return;
    }

    for (i = 3; i >= 0; i--)
    {
        if ((negative != 0U) && (i == 0))
        {
            out[i] = 0x40U; /* minus */
        }
        else
        {
            out[i] = segDigits[magnitude % 10U];
            magnitude /= 10U;
        }
    }

    /* TM1637_DisplayRaw() permanently enables the centre colon. */
    TM1637_DisplayRawWithColon(dev, out, colonOn);
}

/* =====================================================
   CHARACTER GLYPHS
   ===================================================== */

static uint8_t TM1637_Glyph(char c)
{
    switch (c)
    {
        case '0': return 0x3FU;
        case '1': return 0x06U;
        case '2': return 0x5BU;
        case '3': return 0x4FU;
        case '4': return 0x66U;
        case '5': return 0x6DU;
        case '6': return 0x7DU;
        case '7': return 0x07U;
        case '8': return 0x7FU;
        case '9': return 0x6FU;

        case 'A':
        case 'a': return 0x77U;

        case 'B':
        case 'b': return 0x7CU;

        case 'C':
        case 'c': return 0x39U;

        case 'D':
        case 'd': return 0x5EU;

        case 'E':
        case 'e': return 0x79U;

        case 'F':
        case 'f': return 0x71U;

        case 'G':
        case 'g': return 0x6FU;

        case 'H':
        case 'h': return 0x76U;

        case 'I':
        case 'i': return 0x06U;

        /* Seven-segment approximations used by ADJ and AL-V labels. */
        case 'J':
        case 'j': return 0x1EU;

        case 'V':
        case 'v': return 0x1CU;

        case 'L':
        case 'l': return 0x38U;

        case 'T':
        case 't': return 0x78U;

        case 'M':
        case 'm': return 0x37U;

        case 'N':
        case 'n': return 0x54U;

        case 'O':
        case 'o': return 0x5CU;

        case 'P':
        case 'p': return 0x73U;

        case 'R':
        case 'r': return 0x50U;

        case 'S':
        case 's': return 0x6DU;

        case 'U':
        case 'u': return 0x3EU;

        /* Approximation for W on a seven-segment display. */
        case 'W':
        case 'w': return 0x2AU;

        case '-': return 0x40U;

        case ' ': return 0x00U;

        default:  return 0x00U;
    }
}

/* =====================================================
   DISPLAY TEXT
   ===================================================== */

void TM1637_DisplayText(TM1637_Handle_t *dev,
                        const char text[4])
{
    TM1637_DisplayTextWithColon(dev, text, 1U);
}

void TM1637_DisplayTextWithColon(TM1637_Handle_t *dev,
                                 const char text[4],
                                 uint8_t colonOn)
{
    uint8_t data[4];
    uint8_t i;

    if ((dev == NULL) || (text == NULL))
    {
        return;
    }

    for (i = 0; i < 4U; i++)
    {
        data[i] = TM1637_Glyph(text[i]);
    }

    TM1637_DisplayRawWithColon(dev, data, colonOn);
}
