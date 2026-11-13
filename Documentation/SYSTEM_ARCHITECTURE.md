# TTJW System Architecture

3-wire PT100 → analogue front end → ADS1220 24-bit ADC → SPI1 →
STM32L476RG. The firmware converts ADC code to resistance and temperature,
applies SFT, then shares corrected Celsius PV with the display, alarm
evaluator and PID.

Two four-digit TM1637 modules display PV and SV/menu parameters. A 2×2
matrix keypad provides S1 increment/previous, S2 global 3-second HOME exit,
S3 decrement/next and S4 select/confirm.

The adjustment menu contains UNIT, AL-T, AL-V and SFT. The service menu
contains frontend-only ATO/CLB (Auto Calibration) and RST/SET (Reset
Settings) selections.

The PID output is passed to TIM1 CH1 PWM / MOSFET heater driver only when
the compile-time heater safety gate is explicitly enabled. The supplied
configuration keeps that gate disabled by default.
