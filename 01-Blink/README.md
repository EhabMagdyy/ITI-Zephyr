# Zephyr Multi-Threaded LED Blink for STM32/ESP32

Blinks 3 LEDs simultaneously at different rates using Zephyr RTOS threads.

---

## How It Works

Each LED runs in its own thread, toggling independently with no blocking effect on the others.

| Thread | LED Alias | Blink Interval |
|---|---|---|
| `leda0_thread` | `leda0` | 1000 ms |
| `leda1_thread` | `leda1` | 800 ms |
| `leda2_thread` | `leda2` | 1200 ms |

---

## Code Walkthrough

### 1. GPIO Binding (Device Tree)
```c
static const struct gpio_dt_spec thd1_led = GPIO_DT_SPEC_GET(DT_ALIAS(leda0), gpios);
```
Pin info (port, number, polarity) is pulled from the board's `.dts` file at compile time via the `leda0/1/2` aliases — no hardcoded pin numbers.

### 2. Thread Stack Allocation
```c
K_THREAD_STACK_DEFINE(thread1_stack, STACK_SIZE); // 1024 bytes
struct k_thread thread1_leda0;
```
Each thread gets its own **1 KB stack** allocated at compile time, plus a control block struct that Zephyr uses to manage it.

### 3. Thread Functions
```c
void leda0_thread(void *arg1, void *arg2, void *arg3) {
    gpio_pin_configure_dt(&thd1_led, GPIO_OUTPUT_ACTIVE);
    while(1) {
        gpio_pin_toggle_dt(&thd1_led);
		printk("Thread 0\n");
        k_msleep(100);
    }
}
```
Each thread configures its pin as output, then loops forever — toggling the LED and sleeping. `k_msleep()` yields the CPU to the scheduler instead of busy-waiting.

### 4. Thread Creation in `main()`
```c
k_thread_create(&thread1_leda0, thread1_stack, STACK_SIZE,
                leda0_thread, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
```
All 3 threads are created at the same priority (`7`) and start immediately (`K_NO_WAIT`). `main()` then sleeps forever — its only job was to set everything up.

---

## Workflow

```
main()
 ├── Check all GPIOs are ready
 ├── k_thread_create → leda0_thread (toggles every 100ms)
 ├── k_thread_create → leda1_thread (toggles every  80ms)
 ├── k_thread_create → leda2_thread (toggles every 120ms)
 └── k_sleep(K_FOREVER)  ← main parks here

Zephyr Scheduler runs all 3 threads concurrently
```

---

## A) Build & Flash for STM32

### source if you dont want to pass `-DPython...`
``` bash 
source /home/ehab/zephyrproject/zephyr/.venv/bin/activate
```

### Build & give it an overlay
``` bash
west build -p always -b blackpill_f401cc . --extra-dtc-overlay board/stm32f4.overlay -DPython3_EXECUTABLE=/home/ehab/zephyrproject/.venv/bin/python3
```

### Flash
``` bash
# Press NRST Button
west flash -r openocd
# Release after ~1sec
```

## B) Build & Flash for ESP32

### Build & give it an overlay
``` bash
west build -p always -b esp32_devkitc/esp32/procpu . --extra-dtc-overlay board/esp32.overlay -DPython3_EXECUTABLE=/home/ehab/zephyrproject/.venv/bin/python3
```
```
| Property          | `procpu` (Core 0) | `appcpu` (Core 1) |
|------------------ |:-----------------:|:-----------------:|
| **Full name**     | Protocol CPU | Application CPU |
| **Role**          | Main core, boots first, runs WiFi/BT stack | Secondary core, started by `procpu` |
| **Use in Zephyr** | ✅ Default — run your app here | ⚠️ Rarely used standalone |
```

### Flash
``` bash
west flash --esp-device /dev/ttyUSB0
```

### Monitor
``` bash
sudo apt install screen
screen /dev/ttyUSB0 115200
# to terminate
# ctrl + A + X
# from another terminal run
sudo pkill -9 -f "screen dev/ttyUSB0 115200"
```