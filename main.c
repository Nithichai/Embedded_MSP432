#include "msp.h"
#include "string.h"
#define SYSTICK_STCSR (*((volatile unsigned long *)0xE000E010))
#define SYSTICK_STRVR (*((volatile unsigned long *)0xE000E014))
#define SYSTICK_STCVR (*((volatile unsigned long *)0xE000E018))

void SysTick_Init(void);
void SysTick_wait(uint32_t delay);
void SysTick_wait1ms(uint32_t delay);
void Port_Init(void);
void UART0_init(void);
void UART0_send(char c);

int main(void) {
	Port_Init();
	SysTick_Init();
	__disable_irq();
	UART0_init();
	NVIC_EnableIRQ(PORT5_IRQn);
	NVIC_EnableIRQ(PORT6_IRQn);
	NVIC_EnableIRQ(EUSCIA0_IRQn);
	__enable_irq();
	while (1) {}
}

void Port_Init(void) {
	P5SEL1 = 0x00;
	P5SEL0 = 0x00;
	P5DIR = 0x00;
	P5REN = 0x01;
	P5OUT = 0x01;
	P5IES = 0x01;
	P5IFG = 0;
	P5IE = 0x01;

  P6SEL1 = 0x00;
	P6SEL0 = 0x00;
	P6DIR = 0x00;
	P6REN = 0x01;
	P6OUT = 0x01;
	P6IES = 0x01;
	P6IFG = 0;
	P6IE = 0x01;
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

void PORT5_IRQHandler(void) {
	if (P5->IFG & 0x01) {
		SysTick_wait1ms(10);
		if (!(P5IN & 0x01)) {
			char send[] = "SW1 ACTIVE";
			for (int i = 0; i < strlen(send); i++) {
				UART0_send(send[i]);
			}
			UART0_send('\n');
		}
		P5->IFG &= ~0x01;
	}
}

void PORT6_IRQHandler(void) {
	if (P6->IFG & 0x01) {
		SysTick_wait1ms(10);
		if (!(P6IN & 0x01)) {
			char send[] = "SW2 ACTIVE";
			for (int i = 0; i < strlen(send); i++) {
				UART0_send(send[i]);
			}
			UART0_send('\n');
		}
		P6->IFG &= ~0x01;
	}
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
