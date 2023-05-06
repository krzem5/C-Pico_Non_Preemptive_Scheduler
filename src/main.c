#include <hardware/gpio.h>
#include <hardware/watchdog.h>
#include <pico/bootrom.h>
#include <pico/time.h>
#include <scheduler/scheduler.h>



int main(void){
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN,GPIO_OUT);
	scheduler_init();
	gpio_put(PICO_DEFAULT_LED_PIN,0);
	reset_usb_boot(0,0);
}
