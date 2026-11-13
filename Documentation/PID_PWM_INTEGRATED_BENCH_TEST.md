# TTJW Integrated PT100 → ADS1220 → PID → PWM Bench Test

## Purpose
This dedicated STM32L476RG test project exercises the real measurement and control chain:
PT100 → ADS1220 → SPI → PT100 conversion → PV display → PID (SV = 37.0 °C) → TIM1_CH1 PWM on PA8.

This is a bench-validation build, not the production heater firmware. The keypad is disabled and the setpoint is fixed at 37.0 °C. The display unit is fixed to Celsius and SFT is fixed to 0.0 °C so the displayed PV is the unadjusted PT100-derived value.

## CRITICAL SAFETY CONDITION
**Disconnect the heater power supply and heater load before flashing/running this test.** PA8 is enabled to observe the PID-generated logic-level PWM only. Do not connect PA8 directly to a heater. Do not energize the heater power stage during this test. The independent thermal cutoff and power-stage validation are not established by this firmware test.

## Existing pin connections
| Function | STM32L476RG pin |
|---|---|
| ADS1220 CS | PA0 |
| ADS1220 DRDY | PB1 |
| ADS1220 SCLK | PA5 |
| ADS1220 DOUT | PA6 |
| ADS1220 DIN | PA7 |
| PV TM1637 CLK | PA9 |
| PV TM1637 DIO | PA10 |
| SV TM1637 CLK | PB8 |
| SV TM1637 DIO | PB9 |
| PID PWM output (TIM1_CH1) | PA8 |
| Alarm LED | PB10 |

Keep the existing ADS1220/PT100 analogue wiring and module supply arrangement exactly as defined by the verified project schematic and hardware pinout document. This project does not redefine the analogue terminal wiring.

## Expected operation
1. Power the MCU, ADS1220 and displays with the heater supply/load disconnected.
2. SV display shows 37.00 °C. PV display shows the live PT100-derived temperature. If the ADC read times out or the calculated resistance is outside the configured 50–300 Ω plausibility range, the display shows `Err` and PA8 duty is forced to 0%.
3. If PV is below 37.0 °C, the existing controller calculates a proportional demand using Kp = 30 %/°C, Ki = 0 and Kd = 0. Output is clamped to 0–100% and quantized to 10% steps.
4. If PV is at or above 37.0 °C, the controller forces 0% output.
5. Observe PA8 with a 3.3 V-compatible logic analyzer referenced to MCU GND.

## PWM timing
The existing TIM1 setup uses PSC = 7999 and ARR = 19999. With the configured 80 MHz TIM1 clock, the counter is 10 kHz and the PWM period is 2 seconds (0.5 Hz). This is slow time-proportioning output, not a conventional high-frequency switching PWM. Duty is updated in 10% steps.

## PID limitations
This test exercises the controller currently present in the project; it is proportional-only (Kp = 30, Ki = 0, Kd = 0), not a tuned full PID controller. Do not use the resulting output to control a real heater until the control law, sampling/update timing, power stage, independent over-temperature protection, sensor fault response and system-level thermal behaviour are separately validated.

## Validation checklist
- [ ] Confirm heater supply and heater element are physically disconnected.
- [ ] Confirm SV display reads 37.00.
- [ ] Confirm PV changes when the PT100 temperature changes.
- [ ] Compare the PV reading against a calibrated reference thermometer.
- [ ] With PV below 37 °C, verify PA8 shows a non-zero duty command consistent with the controller.
- [ ] As PV approaches 37 °C, verify duty decreases.
- [ ] At/above 37 °C, verify PA8 becomes 0%.
- [ ] Disconnect/open the sensor safely and verify `Err` plus 0% output.
- [ ] Record raw ADC, calculated resistance, PV, PID demand and measured PA8 duty for each test point.

## Important source-derived limitations
The existing PT100 resistance equation and ADS1220 register configuration have been retained unchanged. Their correctness for the physical 3-wire RTD analogue topology must be verified against the actual schematic and calibrated measurements. The firmware's 50–300 Ω range is only a basic plausibility check, not a certified safety limit.
