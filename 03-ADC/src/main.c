#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/adc.h>

#define STACK_SIZE 		1024
#define PRIORITY 		7

static const struct pwm_dt_spec pwm_spec = PWM_DT_SPEC_GET(DT_ALIAS(pwma1));
static const struct adc_dt_spec adc_spec = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));

K_MSGQ_DEFINE(adc_pwm_msgq, sizeof(uint8_t), 1, sizeof(uint8_t));

K_THREAD_STACK_DEFINE(pwmThreadStack, STACK_SIZE);
K_THREAD_STACK_DEFINE(adcThreadStack, STACK_SIZE);

struct k_thread pwmThreadTCB;
struct k_thread adcThreadTCB;

void pwmThread(void *arg1, void *arg2, void *arg3){
    uint32_t period = pwm_spec.period;
    uint32_t pulse = 0;
    uint8_t adcPercentage = 0;

    while(1){
        k_msgq_get(&adc_pwm_msgq, &adcPercentage, K_FOREVER);
        pulse = adcPercentage * (period / 100);
        pwm_set_pulse_dt(&pwm_spec, pulse);
    }
}

void adcThread(void *arg1, void *arg2, void *arg3){
    int ret;

    uint16_t buf;
    struct adc_sequence sequence = {
        .buffer = &buf,
        .buffer_size = sizeof(buf),
    };

    ret = adc_channel_setup_dt(&adc_spec);
    if (ret < 0) {
        return;
    }

    while(1){
        adc_sequence_init_dt(&adc_spec, &sequence);
        ret = adc_read_dt(&adc_spec, &sequence);
        if(ret < 0){
            return;
        }
        else{
            uint8_t percent = (uint8_t)((uint32_t)buf * 100 / ((1 << adc_spec.resolution) - 1));
            k_msgq_put(&adc_pwm_msgq, &percent, K_FOREVER);
        }
    }
}


int main(void){
	if(!pwm_is_ready_dt(&pwm_spec) || !adc_is_ready_dt(&adc_spec)){
        return 0;
    }

    k_thread_create(&pwmThreadTCB, pwmThreadStack, STACK_SIZE, pwmThread, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
    k_thread_create(&adcThreadTCB, adcThreadStack, STACK_SIZE, adcThread, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);

	while(1){
		k_sleep(K_FOREVER);
	}
	return 0;
}
