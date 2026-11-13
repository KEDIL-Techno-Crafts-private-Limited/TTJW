# TTJW Consolidated Firmware Baseline

## Project identity

This is the consolidated STM32CubeIDE project based on the uploaded `TTJW_Firmware_STM32L476RG(3).zip`. The separate `TTJW_3(3).zip` was not used as the application baseline.

## Included functions

- ADS1220 SPI interface (SPI1: PA5/PA6/PA7, CS PA0, DRDY PB1)
- PT100 resistance-to-temperature conversion module
- SFT correction in Celsius
- Alarm modes UP, LO, P-L and OFF
- PID calculation module and TIM1_CH1 PWM driver
- Two TM1637 displays (PV PA9/PA10; SV PB8/PB9)
- 2x2 keypad matrix (PC0-PC3) with finalized S1-S4 sequence
- ADJ and SERV front-end navigation
- Alarm LED PB10

## Finalized keypad sequence

- HOME: S1 increases SV; S3 decreases SV; S4 has no action.
- HOME: short S2 opens ADJ/SERV selection.
- Any screen: hold S2 for 3 seconds to return HOME. A long press does not also trigger the short-press action.
- ADJ: UNIT -> AL-T -> AL-V -> SFT -> UNIT.
- SERV: ATO/CLB and RST/SET are frontend selections only; no calibration/reset backend is executed.
- Menu browsing and editing use S1/S3; S4 enters/confirms.

## Temperature units

All measurement, SFT, alarm and PID calculations are maintained in Celsius. Absolute temperatures are converted for Fahrenheit display using F = C * 1.8 + 32. Temperature differences (alarm deviation and SFT) use deltaF = deltaC * 1.8, without adding 32.

## Heater output safety state

`TTJW_HEATER_OUTPUT_ENABLE` remains `0U`. The PID and PWM modules are present, but the application loop deliberately commands 0% heater duty while this setting is zero. This is intentional: no actual circuit schematic, independent thermal cutoff verification, calibrated temperature validation, or power-stage bench evidence was supplied with the firmware ZIPs. Do not change this define to 1 merely to obtain a waveform.

## Important engineering limitations

- The supplied project configuration sets Kp = 30 and Ki = Kd = 0; therefore the configured controller is proportional-only, not a tuned full PID.
- The configured TIM1 period is a 2-second time-proportioning window based on the project clock assumptions; verify the actual timer clock and power-stage requirements before changing it.
- PT100 accuracy depends on the real 3-wire analog topology, resistor tolerance, IDAC routing, reference wiring, lead matching and calibration. The project ZIP does not include an approved circuit schematic or calibration certificate.
- The two existing TM1637 modules provide four digits each. They do not implement the proposed expanded 6-digit PV / 4-digit SV custom display.
- No claim of target build or physical hardware validation is made by this package. Build with the matching STM32CubeIDE / ARM GNU toolchain, then commission using a calibrated reference and controlled, current-limited test setup.

## Build

1. Extract the ZIP.
2. Open STM32CubeIDE.
3. File -> Import -> Existing Projects into Workspace.
4. Select the extracted `TTJW_Firmware_STM32L476RG` folder.
5. Build the Debug configuration.
6. Resolve any local CubeIDE/toolchain version differences before flashing.

## First bench checks (heater disconnected)

1. Confirm 3.3 V logic supply and common ground.
2. Confirm SWD connection and MCU clock.
3. Verify ADS1220 communication and raw conversion stability.
4. Verify PT100 resistance against a precision resistance reference before temperature calibration.
5. Verify display and keypad functions.
6. Verify PA8 timer waveform using a separate safe diagnostic build with the heater disconnected; do not bypass the production heater inhibit for a connected heater.
7. Validate sensor open/short response and independent thermal cutoff before any heater energization.
