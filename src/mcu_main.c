#include "mcu_api.h"
#include "mcu_errno.h"

// Setup mcu with init_DIG.sh and run that on DIG 7 on linux after downloading this

volatile unsigned long prevTime, curTime;
volatile unsigned long RPM;
// rpm = 60 * (4/(encoder period)) / (cpr * (gear ratio))
// rpm = 240/(encoderPeriod*cpr*gearRatio) -> encoder period in seconds
// rpm = (240*1e6)/(encoderPeriod_us*cpr*gearRatio)

volatile unsigned long duration;

#define CPR			64
#define GEAR_RATIO	30

int my_irq(int req){

	curTime = time_us();
	duration = curTime - prevTime; // in us
	prevTime = curTime;

	RPM = (240*1000*1000) / (duration*CPR*GEAR_RATIO);
//	debug_print(DBG_INFO, "RPM = %lu\n", RPM);

	return IRQ_HANDLED;
}

void mcu_main() {
	gpio_setup(48, 0); // DIG 7 input
	gpio_register_interrupt(48, 1, my_irq); // DIG 7, rising edge, isr

	while (1) {
		debug_print(DBG_INFO, "duration = %u\n", duration);
		mcu_sleep(50); // 500 ms
	}
}
