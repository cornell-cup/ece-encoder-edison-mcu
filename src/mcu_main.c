#include "mcu_api.h"
#include "mcu_errno.h"

volatile unsigned long prevTime, curTime;
volatile unsigned long RPM;
// rpm = 60 * (4/(encoder period)) / (cpr * (gear ratio))
// rpm = 240/(encoderPeriod*cpr*gearRatio) -> encoder period in seconds
// rpm = (240*1e6)/(encoderPeriod_us*cpr*gearRatio)

volatile unsigned long duration;

#define CPR			64
#define GEAR_RATIO	30

#define PWM_PERIOD_MIN	104			// in ns
#define PWM_PERIOD_MAX	218453000	// in ns
#define PWM_PERIOD		1000000		// in ns, = 1ms

#define PWM_PORT		0
#define ENCODER_GPIO	48
volatile int duty_ns;

int my_irq(int req){

	curTime = time_us();
	duration = curTime - prevTime; // in us
	prevTime = curTime;

	RPM = (240*1000*1000) / (duration*CPR*GEAR_RATIO);
//	debug_print(DBG_INFO, "RPM = %lu\n", RPM);

	return IRQ_HANDLED;
}

void mcu_main() {
	gpio_setup(ENCODER_GPIO, 0); // DIG 7 input
	gpio_register_interrupt(ENCODER_GPIO, 1, my_irq); // DIG 7, rising edge, isr

	duty_ns = PWM_PERIOD/4;
	pwm_configure(PWM_PORT, duty_ns, PWM_PERIOD);
	pwm_enable(PWM_PORT);

	while (1) {
		debug_print(DBG_INFO, "duration = %u\n", duration);
		mcu_sleep(50); // 500 ms
	}
}
