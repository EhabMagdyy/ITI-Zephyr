# Zephyr PWM for STM32

## Build & Flash for STM32

## source environment
``` bash 
source /home/ehab/zephyrproject/zephyr/.venv/bin/activate
```

## Build & give it an overlay
``` bash
west build -p always -b blackpill_f401cc . --extra-dtc-overlay board/stm324.overlay -DPython3_EXECUTABLE=/home/ehab/zephyrproject/.venv/bin/python3
```

## Flash
``` bash
# Press NRST Button
west flash -r openocd
# Release after ~1sec
```
