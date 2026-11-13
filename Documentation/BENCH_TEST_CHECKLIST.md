# TTJW Bench Test Checklist

**Keep the heater power disconnected during these tests.**

## Build and hardware
- [ ] Import the project into STM32CubeIDE and perform Clean + Build.
- [ ] Confirm target MCU is STM32L476RG and there are no unresolved symbols.
- [ ] Verify ADS1220 reset, register writes and DRDY conversion timing.
- [ ] Verify PT100 resistance and temperature against a calibrated reference.
- [ ] Verify both TM1637 modules, digit order, brightness and colon control.
- [ ] Verify the physical keypad matrix mapping S1/S2/S3/S4.

## Menu navigation
- [ ] HOME + S3 opens ` ADJ` / ` SET`.
- [ ] S1/S3 toggles between ADJ SET and SERV SET.
- [ ] S4 on ADJ SET opens UNIT.
- [ ] ADJ browse sequence is UNIT → AL-T → AL-V → SFT → UNIT.
- [ ] S1 moves to previous; S3 moves to next parameter.
- [ ] S4 enters parameter editing.
- [ ] S1/S3 change the selected value; S4 confirms and returns to browse.
- [ ] UNIT toggles C/F and does not auto-repeat while held.
- [ ] AL-T cycles UP / LO / P-L / OFF.
- [ ] S4 on SERV SET opens ` ATO` / ` CLB`.
- [ ] S1/S3 toggles to ` RST` / ` SET`.
- [ ] S4 selects and returns from each frontend-only service option.
- [ ] S2 short press has no effect.
- [ ] S2 held continuously for 3 seconds returns HOME from every state.

## Temperature and backend
- [ ] Verify Celsius/Fahrenheit conversion in both directions.
- [ ] Verify SFT correction changes corrected PV by the configured offset.
- [ ] Verify corrected PV is used by display, alarm and PID.
- [ ] Verify UP, LO, P-L and OFF alarm behavior and PB10 output.
- [ ] Simulate ADC timeout, RTD open and RTD short; confirm ERR and 0% heater.
- [ ] Confirm heater PWM output remains disabled while
      TTJW_HEATER_OUTPUT_ENABLE is 0U.
- [ ] Complete independent thermal cutoff and power-stage validation before
      considering enabling the heater output.
