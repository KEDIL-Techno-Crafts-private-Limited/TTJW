# Integration notes

- Started from the PID/PWM integrated bench project (Rev2).
- Restored keypad initialization and keypad processing in the main loop.
- Replaced keypad source with the consolidated full-project implementation, which includes S2 short-press menu entry and S2 long-press HOME return.
- Retained the current ADS1220/PT100 acquisition, two TM1637 displays, alarm logic, PID controller, TIM1 setup and PA8 output.
- ATO/CLB and RST/SET remain frontend-only.
- No claim of target compilation or hardware validation is made.
