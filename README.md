# TTJW Complete Integrated Project – STM32L476RG

## Included functionality
- PT100 3-wire temperature measurement through ADS1220 (SPI1)
- Existing resistance-to-temperature conversion and SFT correction
- PV and SV display using two TM1637 modules
- Complete S1-S4 keypad interface and ADJ/SERV navigation
- ADJ backend: UNIT, AL-T, AL-V and SFT
- Alarm evaluation and PB10 alarm LED
- PID calculation and PA8 / TIM1_CH1 output
- Fixed setpoint initialized to 37.0 °C (adjustable through the HOME screen)
- SERV ATO/CLB and RST/SET are frontend-only selections, as specified

## Pin allocation
- ADS1220: CS PA0, DRDY PB1, SCLK PA5, DOUT PA6, DIN PA7
- PV TM1637: CLK PA9, DIO PA10
- SV TM1637: CLK PB8, DIO PB9
- Keypad: L1 PC0, L2 PC1, R1 PC2, R2 PC3
- Heater PWM: PA8 / TIM1_CH1
- Alarm LED: PB10

## Keypad behaviour
- S1: Increment SV on HOME; previous/increment in menus
- S2 short press: open ADJ/SERV selection from HOME
- S2 hold >= 3 seconds: return HOME from any screen
- S3: Decrement SV on HOME; next/decrement in menus
- S4: Select/enter edit/confirm; no action on HOME
- ADJ cycle: UNIT -> AL-T -> AL-V -> SFT -> UNIT
- AL-T choices: UP -> LO -> P-L -> OFF (S3 next, S1 previous)
- SERV cycle: ATO/CLB <-> RST/SET; frontend only

## PID/PWM test configuration
- Fixed initial SV: 37.0 °C
- Existing PID gains: Kp=30, Ki=0, Kd=0
- TIM1 counter: 10 kHz; ARR=19999; nominal period=2 seconds
- PWM compare calculation retains the one-count low interval at 100% demand
- PID bench-test mode is enabled to permit PA8 logic-level observation.
- The heater power stage must remain physically disconnected during this bench test.
- Above setpoint, the current PID implementation commands 0% output.
- Sensor invalidity commands 0% output.

## Import
Import the extracted project folder into STM32CubeIDE as an existing STM32 project.


## Button sequence correction – Revision 2

- S1 on HOME increases SV.
- S3 on HOME decreases SV (corrected; it no longer opens ADJ).
- S2 short press opens ADJ/SERV selection.
- S1/S3 toggle ADJ SET and SERV SET in the top-level menu.
- S4 selects/enters/confirms; S2 long press returns HOME.

## S2 short-press back navigation – Revision 3

S2 short press is now context-sensitive:
- HOME -> Mode Selection (ADJ SET initially).
- Mode Selection -> HOME.
- Any ADJ parameter/edit or SERV screen -> Mode Selection (ADJ SET initially).

S2 long press (3 seconds) still returns directly HOME from anywhere.
