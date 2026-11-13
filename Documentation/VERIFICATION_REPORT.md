# TTJW Revision 2 — Verification Report

## Static source checks

- 13/13 C source files passed GCC `-fsyntax-only` checks.
- Checks used the STM32L4 HAL/CMSIS headers supplied in this project.
- The modified application modules (`keypad.c`, `ttjw_display.c`,
  `ttjw_state.c`, `tm1637.c` and `main.c`) passed syntax checks.
- No stale `g_displayMode` / `DISPLAY_MODE_*` references remain.

## Host keypad state-machine tests

A temporary host-side test harness mocked the GPIO matrix and HAL tick/delay
functions and exercised the keypad module. The following cases passed:

- S3 from HOME opens ADJ SET.
- S1/S3 toggles ADJ SET and SERV SET.
- S4 enters ADJ parameter browsing.
- S3 advances UNIT to AL-T.
- S4 enters and exits AL-T editing.
- S3 advances alarm type from UP to LO.
- S3 advances AL-T to AL-V.
- S2 short press does nothing.
- UNIT editing toggles Celsius/Fahrenheit.
- SFT next selection wraps to UNIT.
- S4 enters Auto Calibration and service selection advances to Reset Settings.
- S2 long-press returns HOME from ADJ and service states.

## Not performed

- No `arm-none-eabi-gcc` compiler or STM32CubeIDE installation was available.
- No full STM32 target link/build was performed.
- No flashing or physical hardware test was performed.
- No electrical, thermal, clinical or product-safety validation was performed.

A host syntax pass and mocked keypad test do not prove target timing,
electrical operation, ADC accuracy or heater safety. Complete the bench
checklist and a fresh STM32CubeIDE build before hardware use.
