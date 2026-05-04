#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include "drivers/power_switch.h"

#define PWR_SW_NODE DT_NODELABEL(power_switches)

int main(void){
    const struct device *pwr = DEVICE_DT_GET(PWR_SW_NODE);

    if(!device_is_ready(pwr)){
        printk("Power switch not ready!\n");
        return -1;
    }

    while(1){
        power_switch_on(pwr);
        k_msleep(500);
        power_switch_off(pwr);
        k_msleep(500);
    }
    return 0;
}