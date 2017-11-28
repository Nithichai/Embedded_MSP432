#include "msp.h"
#include "string.h"
#define SYSTICK_STCSR (*((volatile unsigned long *)0xE000E010))
#define SYSTICK_STRVR (*((volatile unsigned long *)0xE000E014))
#define SYSTICK_STCVR (*((volatile unsigned long *)0xE000E018))
	
uint8_t isBackward = -1;
char in;

void SysTick_Init(void);
void SysTick_wait(uint32_t delay);
void SysTick_wait1ms(uint32_t delay);
void Port_Init(void);
void Forward_Light(void);
void Backward_Light(void);
void Stop_Light(void);
void UART0_init(void);
void UART0_send(char c);

int main(void) {
	Port_Init();
	SysTick_Init();
	__disable_irq();
	UART0_init();
	NVIC_SetPriority(PORT1_IRQn, 1);
	NVIC_EnableIRQ(PORT1_IRQn);
	NVIC_SetPriority(EUSCIA0_IRQn, 2);
	NVIC_EnableIRQ(EUSCIA0_IRQn);
	__enable_irq();
	while (1) {
		if (isBackward == 0) {
			Forward_Light();
		} else if (isBackward == 1){
			Backward_Light();
		} else {
			Stop_Light();
		}
	}
}

void Port_Init(void) {
	P1SEL1 = 0x00;
	P1SEL0 = 0x00;
	P1DIR = 0x00;
	P1REN = 0x12;
	P1OUT = 0x12;
	P1IES = 0x01;
	P1IFG = 0;
	P1IE = 0x12;
	
	P2SEL1 = 0x00;
	P2SEL0 = 0x00;
	P2DIR = 0x07;
	P2OUT = 0x00;
}

void SysTick_Init(void) {
	SYSTICK_STCSR = 0;
	SYSTICK_STRVR = 0x00FFFFFF;
	SYSTICK_STCVR = 0;
	SYSTICK_STCSR = 0x00000005;
}

void UART0_init(void){
	EUSCI_A0->CTLW0 |= 1;
	EUSCI_A0->MCTLW = 0;
	EUSCI_A0->CTLW0 = 0x0081;
	EUSCI_A0->BRW = 312;
	P1->SEL0 |= 0x0C;
	P1->SEL1 &= ~0x0C;
	EUSCI_A0->CTLW0 &= ~1;
	EUSCI_A0->IE |= 1;
}

void PORT1_IRQHandler(void) {
	if (P1->IFG & 0x02) {
		char buff[] ="SW1 ACTIVE\n";
		for (int i = 0; i < strlen(buff); i++){
			UART0_send(buff[i]);
		}
		isBackward = 0;
		P1->IFG &= ~0x02;
	} else if (P1->IFG & 0x10) {
		char buff[] ="SW2 ACTIVE\n";
		for (int i = 0; i < strlen(buff); i++){
			UART0_send(buff[i]);
		}
		isBackward = 1;
		P1->IFG &= ~0x10;
	}
}

void EUSCIA0_IRQHandler(void){
	in = EUSCI_A0->RXBUF;
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

void UART0_send(char out) {
	while(!(EUSCI_A0->IFG & 0x02)){}
		EUSCI_A0->TXBUF = out;
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

void Stop_Light(void) {
	P2OUT = 0x00;
}
