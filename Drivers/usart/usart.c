#include "usart.h"
#include "utility.h"
#include "main.h"

#define PIN_1       1
#define USART_M1    28
#define USART_M0    12
#define USART_PCE   0
#define USART_TE    3
#define USART_RE    2
#define USART_CTSE  9
#define USART_RTSE  8
#define USART_OVER8 15
#define USART_RXNE  5

void usart1_init(void)
{
    RCC->APBENR2 |= RCC_APBENR2_USART1EN;  // enable USART1 clock
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;     // enable GPIOA clock

    RCC->CCIPR &= ~RCC_CCIPR_USART1SEL;    // clock source = PCLK

    // PA1 = USART1_RX, alternate function, AF4
    GPIOA->MODER &= ~(3U << (PIN_1 * 2));
    GPIOA->MODER |=  (2U << (PIN_1 * 2));

    GPIOA->OSPEEDR &= ~(3U << (PIN_1 * 2));
    GPIOA->OSPEEDR |=  (1U << (PIN_1 * 2));

    GPIOA->OTYPER &= ~(1U << PIN_1);
    GPIOA->PUPDR  &= ~(3U << (PIN_1 * 2));

    GPIOA->AFR[0] &= ~(0xFU << (PIN_1 * 4));
    GPIOA->AFR[0] |=  (0x4U << (PIN_1 * 4));

    USART1->CR1 &= ~USART_CR1_UE;          // disable USART to configure

    USART1->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1);  // 8 data bits
    USART1->CR1 &= ~USART_CR1_PCE;                   // no parity

    USART1->CR1 |= USART_CR1_RE;           // enable receiver

    USART1->CR3 &= ~(USART_CR3_CTSE | USART_CR3_RTSE);  // no hardware flow control

    USART1->BRR = 12000000UL / 9600UL;     // 9600 baud @ 12 MHz PCLK

    USART1->CR1 |= USART_CR1_UE;           // enable USART
}

int usart1_read(uint8_t *byte, uint32_t timeout_ms)
{
    uint32_t start_tick = get_systick_ms();

    while (!(USART1->ISR & (1UL << USART_RXNE))) {  // wait for received byte
        if (USART1->ISR & USART_ISR_ORE) {
            USART1->ICR |= USART_ICR_ORECF;  // clear overrun flag
            return -2;                        // overrun error
        }

        if ((get_systick_ms() - start_tick) > timeout_ms) {
            return -1;  // timeout
        }
    }

    if (USART1->ISR & USART_ISR_ORE) {
        USART1->ICR |= USART_ICR_ORECF;  // clear any late overrun flag
    }

    *byte = (uint8_t)(USART1->RDR & 0xFFU);  // read received byte

    return 0;
}