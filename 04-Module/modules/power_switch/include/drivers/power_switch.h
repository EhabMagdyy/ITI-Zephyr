#ifndef ZEPHYR_INCLUDE_DRIVERS_POWER_SWITCH_H_
#define ZEPHYR_INCLUDE_DRIVERS_POWER_SWITCH_H_

#include <zephyr/device.h>
#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

// Driver API: function pointer table
typedef int (*power_switch_on_t)(const struct device *dev);
typedef int (*power_switch_off_t)(const struct device *dev);
typedef int (*power_switch_get_state_t)(const struct device *dev, bool *state);

__subsystem struct power_switch_driver_api {
    power_switch_on_t on;
    power_switch_off_t off;
    power_switch_get_state_t get_state;
};

/* Public API wrappers */
static inline int power_switch_on(const struct device *dev)
{
    const struct power_switch_driver_api *api = dev->api;
    return api->on(dev);
}

static inline int power_switch_off(const struct device *dev)
{
    const struct power_switch_driver_api *api = dev->api;
    return api->off(dev);
}

static inline int power_switch_get_state(const struct device *dev, bool *state)
{
    const struct power_switch_driver_api *api = dev->api;
    return api->get_state(dev, state);
}

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_DRIVERS_POWER_SWITCH_H_ */