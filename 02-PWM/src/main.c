#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>

#define STACK_SIZE 		1024
#define PRIORITY 		7

static const struct pwm_dt_spec pwm1_spec = PWM_DT_SPEC_GET(DT_ALIAS(pwma1));
static const struct pwm_dt_spec pwm2_spec = PWM_DT_SPEC_GET(DT_ALIAS(pwma2));
static const struct pwm_dt_spec pwm3_spec = PWM_DT_SPEC_GET(DT_ALIAS(pwma3));

K_THREAD_STACK_DEFINE(thread1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread2_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread3_stack, STACK_SIZE);

struct k_thread thread1TCB;
struct k_thread thread2TCB;
struct k_thread thread3TCB;

void pwm1_thread(void *arg1, void *arg2, void *arg3){
    uint32_t period = pwm1_spec.period;
    uint32_t step = period / 100;
    uint32_t pulse = 0;
    uint8_t direction = 0;

    while(1){
        if(direction == 0){
            pulse += step;
            if(pulse >= period){
                pulse = period;
                direction = 1;
            }
        }
		else {
            pulse -= step;
            if(pulse <= step){
                pulse = 0;
                direction = 0;
            }
        }

        pwm_set_pulse_dt(&pwm1_spec, pulse);
        k_msleep(5);
    }
}

void pwm2_thread(void *arg1, void *arg2, void *arg3){
    uint32_t period = pwm2_spec.period;
    uint32_t step = period / 100;
    uint32_t pulse = 0;
    uint8_t direction = 0;

    while(1){
        if(direction == 0){
            pulse += step;
            if(pulse >= period){
                pulse = period;
                direction = 1;
            }
        }
		else {
            pulse -= step;
            if(pulse <= step){
                pulse = 0;
                direction = 0;
            }
        }

        pwm_set_pulse_dt(&pwm2_spec, pulse);
        k_msleep(2);
    }
}

void pwm3_thread(void *arg1, void *arg2, void *arg3){
    uint32_t period = pwm3_spec.period;
    uint32_t step = period / 100;
    uint32_t pulse = 0;
    uint8_t direction = 0;

    while(1){
        if(direction == 0){
            pulse += step;
            if(pulse >= period){
                pulse = period;
                direction = 1;
            }
        }
		else {
            pulse -= step;
            if(pulse <= step){
                pulse = 0;
                direction = 0;
            }
        }

        pwm_set_pulse_dt(&pwm3_spec, pulse);
        k_msleep(3);
    }
}

int main(void){
	if(!pwm_is_ready_dt(&pwm1_spec) || !pwm_is_ready_dt(&pwm2_spec) || !pwm_is_ready_dt(&pwm3_spec)){
		printk("PWM not ready!\n");
        return 0;
    }
	printk("PWM ready!\n");

    k_thread_create(&thread1TCB, thread1_stack, STACK_SIZE, pwm1_thread, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
	k_thread_create(&thread2TCB, thread2_stack, STACK_SIZE, pwm2_thread, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
	k_thread_create(&thread3TCB, thread3_stack, STACK_SIZE, pwm3_thread, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);

	while(1){
		k_sleep(K_FOREVER);
	}
	return 0;
}
