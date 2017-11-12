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
	while (1) {
		if (P1OUT == 0x02) {
			Forward_Light();
		} else if (P1OUT == 0x10) {
			Backward_Light();
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
	while ((SYSTICK_STCSR & 0x000100000) == 0) {
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
	P2OUT = 0x00;
	for (int i = 0; i < 3; i++) {
		P2OUT = P2OUT << 1;
		SysTick_wait(1000);
	}
}

void Backward_Light(void) {
	P2OUT = 0x04;
	for (int i = 0; i < 3; i++) {
		P2OUT = P2OUT >> 1;
		SysTick_wait(1000);
	}
}