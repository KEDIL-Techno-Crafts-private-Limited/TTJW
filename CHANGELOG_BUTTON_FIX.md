# Button Sequence Correction – Revision 2

## Change
Corrected the S3 action on the HOME screen.

- S1 on HOME: increase setpoint.
- S3 on HOME: decrease setpoint.
- S2 short press on HOME: open top-level selection, initially ADJ SET.
- S1/S3 in top-level selection: toggle between ADJ SET and SERV SET.
- S4: select/enter/confirm.
- S2 long press (3 seconds): return HOME from any UI state.
- S4 on HOME: no action.

## Scope
Only the HOME-screen S3 branch and keypad module documentation were changed.
The existing ADJ/SERV navigation, parameter editing, sensor acquisition, PID/PWM,
display drivers, pin assignments, and CubeMX configuration were retained.

## Validation
Source-level assertions and ZIP integrity were checked. A target STM32CubeIDE
compile and physical keypad test are still required.
