

#include "../int/timer0/timer0.h"

volatile uint16_t msCount;
void timeloop(uint16_t timeLeft) {
	// 1ms time base
	// set timer0 with 64 prescaller
	t0ClockSelect(T0_PRESCALER_64);
	operationMode(T0_OPMODE_CTC);
	enableOC0Interrupt();
	OCR0=250;
	msCount = timeLeft;
}

struct timeCounters {
	uint16_t Cntr0;
	uint16_t Cntr1;
	uint16_t Cntr2;
	uint16_t Cntr3;
	uint16_t Cntr4;
	uint16_t Cntr5;
	uint16_t Cntr6;
	uint16_t Cntr7;
} time;


void timer0_comp_callback() {
	if(time.Cntr0 > 0) {
		ime.Cntr0--;
	}
	if(time.Cntr1 > 0) {
		ime.Cntr1--;
	}
	if(time.Cntr2 > 0) {
		ime.Cntr2--;
	}
	if(time.Cntr3 > 0) {
		ime.Cntr3--;
	}
	if(time.Cntr4 > 0) {
		ime.Cntr4--;
	}
	if(time.Cntr5 > 0) {
		ime.Cntr5--;
	}
	if(time.Cntr6 > 0) {
		ime.Cntr6--;
	}
	if(time.Cntr7 > 0) {
		ime.Cntr7--;
	}
}

uint16_t setMs(uint16_t value, MS_1) {

} 
