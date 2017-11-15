#include "msp.h"
#define SYSTICK_STCSR (*((volatile unsigned long *)0xE000E010))
#define SYSTICK_STRVR (*((volatile unsigned long *)0xE000E014))
#define SYSTICK_STCVR (*((volatile unsigned long *)0xE000E018))
	
void SysTick_Init(void);
void SysTick_wait(uint32_t delay);
void SysTick_wait1ms(uint32_t delay);
void Port_Init(void);
void Forward_Light(void);
void Backward_Light(void);

int main(void) {
	SysTick_Init();
	Port_Init();
	uint8_t Input_SW_P1_1, Input_SW_P1_4;
	uint8_t isBackward;
	
	while (1) {
		Input_SW_P1_1 = P1IN & 0x02;
		Input_SW_P1_4 = P1IN & 0x10;
		if (Input_SW_P1_1 == 0x00) {
			isBackward = 0;
		} else if (Input_SW_P1_4 == 0x00) {
			isBackward = 1;
		}
		if (isBackward == 0) {
			Forward_Light();
			// P2OUT = 0x01;
		} else {
			Backward_Light();
			// P2OUT = 0x03;
		}
	}
}

void SysTick_Init(void) {
	SYSTICK_STCSR = 0;
	SYSTICK_STRVR = 0x00FFFFFF;
	SYSTICK_STCVR = 0;
	SYSTICK_STCSR = 0x00000005;
}

void SysTick_wait(uint32_t delay) {
	SYSTICK_STRVR = delay - 1;
	SYSTICK_STCVR = 0;
	while ((SYSTICK_STCSR & 0x00010000) == 0) {
	}
}

void SysTick_wait1ms(uint32_t delay) {
	uint32_t i;
	for (i = 0; i < delay; i++) {
		SysTick_wait(3000);
	}
}

void Port_Init(void) {
	P1SEL1 = 0x00;
	P1SEL0 = 0x00;
	P2SEL1 = 0x00;
	P2SEL0 = 0x00;
	P1DIR = 0x00;
	P1REN = 0x12;
	P1OUT = 0x12;
	P2DIR = 0x07;
	P2OUT = 0x00;
}

void Forward_Light(void) {
	P2OUT = 0x01;
	SysTick_wait1ms(1000);
	uint8_t i;
	for (i = 0; i < 2; i++) {
		P2OUT = P2OUT << 1;
		SysTick_wait1ms(1000);
	}
}

void Backward_Light(void) {
	P2OUT = 0x04;
	SysTick_wait1ms(1000);
	uint8_t i;
	for (i = 0; i < 2; i++) {
		P2OUT = P2OUT >> 1;
		SysTick_wait1ms(1000);
	}
}
