# Source Baseline

This revision is based on the user-provided
`TTJW_Firmware_STM32L476RG (1).zip`. The STM32CubeIDE project structure,
STM32L476RG target, CubeMX `.ioc`, startup file, HAL/CMSIS drivers, linker
scripts and existing hardware pin definitions were retained.

The user interface was revised to implement the ADJ SET / SERV SET menu,
UNIT / AL-T / AL-V / SFT parameter sequence, ATO/CLB and RST/SET frontend
service choices, S4 selection/edit confirmation and the global S2 3-second
HOME exit.

The ADS1220, PT100 conversion, Celsius/Fahrenheit processing, SFT-corrected
PV, alarm evaluation, PB10 indicator, PID and heater-control modules were
retained. Heater output remains inhibited by default.

No physical board test or authoritative STM32CubeIDE ARM build was possible
in the preparation environment. Review the root README and bench checklist
before programming or connecting the heater.
