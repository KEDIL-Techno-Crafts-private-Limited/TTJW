
#ifndef TM1637_H
#define TM1637_H

#include <stdint.h>
#include "main.h"

typedef struct
{
    GPIO_TypeDef *clkPort;
    uint16_t clkPin;

    GPIO_TypeDef *dioPort;
    uint16_t dioPin;

} TM1637_Handle_t;

/* Initialization */
void TM1637_Init(TM1637_Handle_t *dev);

/* Brightness: 0 to 7 */
void TM1637_SetBrightness(TM1637_Handle_t *dev, uint8_t brightness);

/* Display four raw segment patterns */
void TM1637_DisplayRaw(TM1637_Handle_t *dev, const uint8_t digits[4]);
void TM1637_DisplayRawWithColon(TM1637_Handle_t *dev,
                                const uint8_t digits[4],
                                uint8_t colonOn);

/* Display integer with optional decimal point
 * decimalPosition: 0 = thousands digit
 *                  1 = hundreds digit
 *                  2 = tens digit
 *                  3 = units digit
 *                  255 = no decimal point
 */
void TM1637_DisplayNumber(TM1637_Handle_t *dev,
                          int32_t value,
                          uint8_t decimalPosition);

/* Display a signed value in XX.XX format.
 * The centre colon is kept on by TM1637_DisplayRaw().
 * valueHundredths=3700 displays 37:00.
 */
void TM1637_DisplayFixed2(TM1637_Handle_t *dev, int32_t valueHundredths);
void TM1637_DisplayFixed2WithColon(TM1637_Handle_t *dev,
                                   int32_t valueHundredths,
                                   uint8_t colonOn);

/* Display four characters */
void TM1637_DisplayText(TM1637_Handle_t *dev, const char text[4]);
void TM1637_DisplayTextWithColon(TM1637_Handle_t *dev,
                                 const char text[4],
                                 uint8_t colonOn);

#endif
