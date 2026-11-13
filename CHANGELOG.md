# Change Log

## Revision 2 — Updated button sequence and display menus

- Replaced the previous linear S2 menu navigation with the ADJ SET / SERV SET
  top-level menu.
- Implemented S1 previous/increment and S3 next/decrement behavior.
- Reserved S2 for a global 3-second long-press return to HOME; short S2 press
  has no action.
- Implemented S4 selection, entry into parameter editing and confirmation
  back to browsing.
- Implemented ADJ sequence UNIT → AL-T → AL-V → SFT → UNIT.
- Added SERV frontend choices ` ATO` / ` CLB` and ` RST` / ` SET`.
- Added seven-segment glyph approximations for J and V.
- Updated menu specification, README and bench-test checklist.

## Backend retained

- Kept ADS1220, PT100 conversion, Celsius/Fahrenheit handling, SFT-corrected
  PV, alarm comparisons, PB10 indicator and PID/heater-control modules.
- Heater output remains inhibited by default through
  `TTJW_HEATER_OUTPUT_ENABLE = 0U`.

## Verification limits

All application C files passed host-GCC syntax-only checks against the supplied
STM32 HAL/CMSIS headers. The STM32 ARM target toolchain / CubeIDE and physical
hardware were not available for a fresh target build or hardware test. See
`README.md` and `Documentation/BENCH_TEST_CHECKLIST.md`.
