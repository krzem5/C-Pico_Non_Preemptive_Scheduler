#include <hardware/gpio.h>
#include <hardware/watchdog.h>
#include <pico/bootrom.h>
#include <pico/time.h>
#include <scheduler/scheduler.h>



static scheduler_task_id_t task2_id=SCHEDULER_INVALID_TASK_ID;



static scheduler_task_delay_t task2(void* ctx){
	// Never executes
	while (1);
	return 0;
}



static scheduler_task_delay_t task1(void* ctx){
	if (scheduler_unschedule(task2_id)){
		gpio_put(PICO_DEFAULT_LED_PIN,0);
		reset_usb_boot(0,0);
		return 0;
	}
	gpio_put(PICO_DEFAULT_LED_PIN,1);
	task2_id=scheduler_schedule(1000000,task2,NULL);
	return 500000;
}



int main(void){
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN,GPIO_OUT);
	watchdog_enable(500,0);
	scheduler_init();
	scheduler_schedule(100,task1,NULL);
	while (1){
		watchdog_update();
		scheduler_update();
	}
	return 0;
}
