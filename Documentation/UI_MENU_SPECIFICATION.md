# TTJW UI Menu Specification — Revision 2

## Button assignment

- S1: increment / previous
- S2: 3-second long-press to HOME globally; short press ignored
- S3: decrement / next; from HOME opens top-level menu
- S4: select / enter edit / confirm

## State sequence

```text
HOME
  -- S3 short -->
MENU: ADJ / SET  <---- S1 or S3 toggle ---->  MENU: SERV / SET
  -- S4 -->                                      -- S4 -->
ADJ parameter browse                         SERV choice browse
  UNIT                                         ATO / CLB
  AL-T                                         RST / SET
  AL-V
  SFT
  (loop to UNIT)

For each ADJ parameter:
  browse -- S4 --> edit
  edit -- S4 --> browse

For each service choice:
  browse -- S4 --> selected (frontend-only)
  selected -- S4 --> browse

From every state:
  S2 held continuously for 3000 ms --> HOME
```

## Display strings

| State | PV module | SV module | Colon |
|---|---|---|---|
| HOME | Corrected PV `XX:XX` | SV `XX:XX` | ON |
| Top menu ADJ | ` ADJ` | ` SET` | OFF |
| Top menu SERV | `SERV` | ` SET` | OFF |
| UNIT | `UNIT` | `   C` / `   F` | OFF |
| AL-T | `AL-T` | Alarm mode | OFF |
| AL-V | `AL-V` | Deviation | SV numeric colon ON |
| SFT | `SFT ` | Correction | SV numeric colon ON |
| Auto Calibration | ` ATO` | ` CLB` | OFF |
| Reset Settings | ` RST` | ` SET` | OFF |
| Sensor fault | `Err ` | blank | OFF |

## Editing and navigation

- ADJ browsing: S1 previous, S3 next, S4 enters edit.
- ADJ editing: S1 increases, S3 decreases, S4 confirms and returns to browse.
- UNIT: S1/S3 toggle C/F; auto-repeat is disabled for this binary choice.
- AL-T: S3 advances UP → LO → P-L → OFF → UP; S1 reverses.
- AL-V and SFT: S1/S3 adjust the selected value.
- SERV browsing: S1/S3 toggles Auto Calibration / Reset Settings.
- SERV selected: no backend operation; S4 returns to service browsing.
- S2 short press: no action.
- S2 long press: global HOME exit.

## Notes

The 4-digit TM1637 modules cannot display arbitrary words. J and V use
seven-segment approximations. The service labels are exactly ` ATO` / ` CLB`
and ` RST` / ` SET`, as requested.
