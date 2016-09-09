
/****************** description ******************
This library makes use of Timer2 and generates fast PWM
currently to keep stuff simple the no prescalling is applied
so timer clock is same as system clock.

The functions are self explainatory  from their names
-------------------------------------------------*/
void fastPWM2Init() {
	// WGM21:0 = 2  sets fastPWM mode
	TCCR2 |= (1<<WGM21) | (1<<WGM20);
}

void fastPWM2Start() {
	// CS22:0 = 1 means no prescaling. Timer clk = system clk
	TCCR2 |= (1<<CS20);
	// COM21:0 = 2	non inverting PWM on OC2 (PD7)
	TCCR2 |= (1<<COM21);
}


void fastPWM2Duty(uint8_t duty) {
	OCR2 = duty;
}

void fastPWM2Stop() {
	//stop timer2
	TCCR2 &= ~(1<<CS20);
	// make PD7 as GPIO again
	TCCR2 &= ~(1<<COM21);
}