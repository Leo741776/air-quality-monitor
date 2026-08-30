#include "spi.h"
#include "stm32c0xx_ll_bus.h"

#define PIN_0 0
#define PIN_3 3
#define PIN_4 4
#define PIN_5 5
#define PIN_7 7

void spi1_init(void)
{
    RCC->APBENR2 |= RCC_APBENR2_SPI1EN;  // enable SPI1 clock
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;   // enable GPIOA clock
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;   // enable GPIOB clock

    // PA5 (SCLK), PA7 (MOSI): alternate function, high speed, push-pull, AF0
    GPIOA->MODER &= ~((3U << (PIN_5 * 2)) | (3U << (PIN_7 * 2)));
    GPIOA->MODER |= ((2U << (PIN_5 * 2)) | (2U << (PIN_7 * 2)));
    GPIOA->OSPEEDR &= ~((3U << (PIN_5 * 2)) | (3U << (PIN_7 * 2)));
    GPIOA->OSPEEDR |= ((3U << (PIN_5 * 2)) | (3U << (PIN_7 * 2)));
    GPIOA->OTYPER &= ~((1U << PIN_5) | (1U << PIN_7));
    GPIOA->PUPDR &= ~((3U << (PIN_5 * 2)) | (3U << (PIN_7 * 2)));
    GPIOA->AFR[0] &= ~(0xFU << (PIN_5 * 4));
    GPIOA->AFR[0] &= ~(0xFU << (PIN_7 * 4));

    // PA4 (RESET): output, low speed, push-pull, no pull, idle HIGH
    GPIOA->MODER &= ~(3U << (PIN_4 * 2));
    GPIOA->MODER |= (1U << (PIN_4 * 2));
    GPIOA->OSPEEDR &= ~(3U << (PIN_4 * 2));
    GPIOA->OTYPER  &= ~(1U << PIN_4);
    GPIOA->PUPDR   &= ~(3U << (PIN_4 * 2));
    GPIOA->BSRR    =  (1U << PIN_4);

    // PA3 (DC): output, low speed, push-pull, no pull, idle HIGH
    GPIOA->MODER   &= ~(3U << (PIN_3 * 2));
    GPIOA->MODER   |=  (1U << (PIN_3 * 2));
    GPIOA->OSPEEDR &= ~(3U << (PIN_3 * 2));
    GPIOA->OTYPER  &= ~(1U << PIN_3);
    GPIOA->PUPDR   &= ~(3U << (PIN_3 * 2));
    GPIOA->BSRR = (1U << PIN_3);

    // PB0 (CS/SS): software-controlled output, low speed, push-pull, no pull, idle HIGH
    GPIOB->MODER &= ~(3U << (PIN_0 * 2));
    GPIOB->MODER |= (1U << (PIN_0 * 2));
    GPIOB->OSPEEDR &= ~(3U << (PIN_0 * 2));
    GPIOB->OTYPER &= ~(1U << PIN_0);
    GPIOB->PUPDR &= ~(3U << (PIN_0 * 2));
    GPIOB->BSRR = (1U << PIN_0);

    SPI1->CR1 &= ~(SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE | SPI_CR1_RXONLY);  // full duplex
    SPI1->CR1 |= SPI_CR1_MSTR;           // master mode
    SPI1->CR1 &= ~SPI_CR1_CPOL;          // clock polarity idle low
    SPI1->CR1 &= ~SPI_CR1_CPHA;          // sample on first clock edge
    SPI1->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);  // software NSS management
    SPI1->CR1 &= ~SPI_CR1_BR_Msk;        // baud rate = fPCLK/2
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST;      // MSB first
    SPI1->CR1 &= ~SPI_CR1_CRCEN;         // CRC disabled

    SPI1->CR2 &= ~SPI_CR2_FRF;           // Motorola SPI protocol
    SPI1->CR2 &= ~SPI_CR2_DS;
    SPI1->CR2 |= (SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0);  // 8-bit data frame

    SPI1->CR1 |= SPI_CR1_SPE;            // enable SPI1
}