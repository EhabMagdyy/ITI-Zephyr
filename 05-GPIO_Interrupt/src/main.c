#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(leda0), gpios);
static const struct gpio_dt_spec ledTimer = GPIO_DT_SPEC_GET(DT_ALIAS(leda2), gpios);
static const struct gpio_dt_spec extInterrupt = GPIO_DT_SPEC_GET(DT_ALIAS(buttona1), gpios);

void button_pressed(const struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins){
	gpio_pin_toggle_dt(&led);
}

struct gpio_callback button_cb_data;

void timer_expiry_function(struct k_timer *timer){
	gpio_pin_toggle_dt(&ledTimer);
}

K_TIMER_DEFINE(led_timer, timer_expiry_function, NULL);

int main(void){
	if(!gpio_is_ready_dt(&extInterrupt) || !gpio_is_ready_dt(&led)){
        return 0;
    }

	gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure_dt(&ledTimer, GPIO_OUTPUT_ACTIVE);

	gpio_pin_configure_dt(&extInterrupt, GPIO_INPUT | GPIO_PULL_DOWN);
	gpio_pin_interrupt_configure_dt(&extInterrupt, GPIO_INT_EDGE_TO_ACTIVE);

	gpio_init_callback(&button_cb_data, button_pressed, BIT(extInterrupt.pin));
	gpio_add_callback(extInterrupt.port, &button_cb_data);

	k_timer_start(&led_timer, K_NO_WAIT, K_SECONDS(1));

	while(1){
		k_sleep(K_FOREVER);
	}
	return 0;
}
