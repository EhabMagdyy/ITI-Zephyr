#define DT_DRV_COMPAT power_switch

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <drivers/power_switch.h>

LOG_MODULE_REGISTER(power_switch, CONFIG_LOG_DEFAULT_LEVEL);

// Per-instance configuration(read-only, from devicetree)
struct power_switch_config {
    struct gpio_dt_spec gpio;
};

// Per-instance runtime data
struct power_switch_data {
    bool state;
};

static int power_switch_api_on(const struct device *dev){
    const struct power_switch_config *cfg = dev->config;
    struct power_switch_data *data = dev->data;
    int ret;

    ret = gpio_pin_set_dt(&cfg->gpio, 1);
    if(ret < 0) {
        LOG_ERR("Failed to set GPIO high: %d", ret);
        return ret;
    }

    data->state = true;
    LOG_INF("Power switch ON");
    return 0;
}

static int power_switch_api_off(const struct device *dev){
    const struct power_switch_config *cfg = dev->config;
    struct power_switch_data *data = dev->data;
    int ret;

    ret = gpio_pin_set_dt(&cfg->gpio, 0);
    if(ret < 0) {
        LOG_ERR("Failed to set GPIO low: %d", ret);
        return ret;
    }

    data->state = false;
    LOG_INF("Power switch OFF");
    return 0;
}

static int power_switch_api_get_state(const struct device *dev, bool *state){
    struct power_switch_data *data = dev->data;
    *state = data->state;
    return 0;
}

static const struct power_switch_driver_api power_switch_api = {
    .on = power_switch_api_on,
    .off = power_switch_api_off,
    .get_state = power_switch_api_get_state,
};

static int power_switch_init(const struct device *dev){
    const struct power_switch_config *cfg = dev->config;
    int ret;

    if(!gpio_is_ready_dt(&cfg->gpio)) {
        LOG_ERR("GPIO device not ready");
        return -ENODEV;
    }

    ret = gpio_pin_configure_dt(&cfg->gpio, GPIO_OUTPUT_INACTIVE);
    if(ret < 0) {
        LOG_ERR("Failed to configure GPIO: %d", ret);
        return ret;
    }

    LOG_INF("Power switch initialized");
    return 0;
}

/* Instantiation macro: creates one device per DT node */
#define POWER_SWITCH_INIT(inst)                                        \
    static struct power_switch_data power_switch_data_##inst;          \
                                                                       \
    static const struct power_switch_config power_switch_cfg_##inst = {\
        .gpio = GPIO_DT_SPEC_INST_GET(inst, gpios),                    \
    };                                                                 \
                                                                       \
    DEVICE_DT_INST_DEFINE(inst,                                        \
                          power_switch_init,                           \
                          NULL,                                        \
                          &power_switch_data_##inst,                   \
                          &power_switch_cfg_##inst,                    \
                          POST_KERNEL,                                 \
                          CONFIG_KERNEL_INIT_PRIORITY_DEVICE,          \
                          &power_switch_api);

DT_INST_FOREACH_STATUS_OKAY(POWER_SWITCH_INIT)