# TTJW Hardware Pinout and Wiring Reference

Target MCU: STM32L476RG (LQFP64 / Nucleo-compatible pin naming as used by project).
This table records the exact application pin allocation requested for this firmware. Confirm package pin numbers against the selected MCU package and PCB schematic before fabrication.

| No. | Hardware | Signal | MCU pin | Direction / notes |
|---:|---|---|---|---|
| 1 | ADS1220 | CS | PA0 | GPIO output, active low |
| 2 | ADS1220 | DRDY | PB1 | GPIO input; data-ready indication |
| 3 | ADS1220 | SCLK | PA5 | SPI1 SCK |
| 4 | ADS1220 | DOUT | PA6 | SPI1 MISO |
| 5 | ADS1220 | DIN | PA7 | SPI1 MOSI |
| 6 | PV TM1637 | CLK | PA9 | GPIO bit-banged clock |
| 7 | PV TM1637 | DIO | PA10 | Bidirectional GPIO data |
| 8 | SV TM1637 | CLK | PB8 | GPIO bit-banged clock |
| 9 | SV TM1637 | DIO | PB9 | Bidirectional GPIO data |
| 10 | Matrix keypad | L1 | PC0 | Matrix scan output |
| 11 | Matrix keypad | L2 | PC1 | Matrix scan output |
| 12 | Matrix keypad | R1 | PC2 | Matrix scan input |
| 13 | Matrix keypad | R2 | PC3 | Matrix scan input |
| 14 | Heater driver | PWM | PA8 | TIM1_CH1 alternate function; logic-level control only |
| 15 | Alarm LED | Control | PB10 | GPIO output |

## Matrix keypad switch mapping

| Switch | Matrix intersection | Meaning |
|---|---|---|
| S1 | L1 (PC0) + R1 (PC2) | Increment / previous |
| S2 | L2 (PC1) + R1 (PC2) | Short: open ADJ/SERV from HOME; hold 3 s: HOME |
| S3 | L1 (PC0) + R2 (PC3) | Decrement / next |
| S4 | L2 (PC1) + R2 (PC3) | Select / enter / confirm; no HOME action |

## TM1637 power and logic

Each module has VCC, GND, CLK and DIO. Connect both grounds to MCU ground. Use a supply voltage supported by the exact module. Check the DIO pull-up voltage: do not allow a 5 V pull-up to drive the STM32 GPIO; use 3.3 V-compatible powering or proper level translation. TM1637 is a proprietary two-wire serial interface, not I2C.

## ADS1220 / PT100 notes

The firmware pin mapping is listed above. ADS1220 AVDD, DVDD, AVSS, DGND, reference pins, AIN pins, IDAC routing, reference resistor and three PT100 leads must follow the approved analog schematic. The MCU pin table alone does not define those analog connections. Project configuration records RREF = 1.624 kOhm and IDAC1 = 500 uA; verify the actual board topology before trusting calculated temperature.

## Heater driver notes

PA8 is a logic-level PWM output and must connect to the gate-driver/control input, never directly to the heater. The heater supply and return belong to the power stage. Independent thermal cutoff and suitable fault protection are required. Heater output remains compile-time inhibited in this delivered project (`TTJW_HEATER_OUTPUT_ENABLE 0U`).

## Debug interface

SWDIO = PA13, SWCLK = PA14, target reference = 3.3 V, GND = common ground, NRST recommended.
