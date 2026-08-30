#include "i2c1.h"
#include "stm32c0xx_ll_rcc.h"

#define PIN_9  9
#define PIN_10 10

void i2c1_init(void)
{
    RCC->CCIPR &= ~RCC_CCIPR_I2C1SEL;    // I2C1 clock source = HSI

    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;   // enable GPIOA clock

    // PA9 (SCL) as alternate function, open-drain, no pull, AF6
    GPIOA->MODER &= ~(3U << (PIN_9 * 2));
    GPIOA->MODER |= (2U << (PIN_9 * 2));
    GPIOA->OSPEEDR &= ~(3U << (PIN_9 * 2));
    GPIOA->OTYPER |= (1U << PIN_9);
    GPIOA->PUPDR &= ~(3U << (PIN_9 * 2));
    GPIOA->AFR[1] &= ~(0xFU << ((PIN_9 - 8) * 4));
    GPIOA->AFR[1] |= (6U << ((PIN_9 - 8) * 4));

    // PA10 (SDA) as alternate function, open-drain, no pull, AF6
    GPIOA->MODER &= ~(3U << (PIN_10 * 2));
    GPIOA->MODER |= (2U << (PIN_10 * 2));
    GPIOA->OSPEEDR &= ~(3U << (PIN_10 * 2));
    GPIOA->OTYPER |= (1U << PIN_10);
    GPIOA->PUPDR &= ~(3U << (PIN_10 * 2));
    GPIOA->AFR[1] &= ~(0xFU << ((PIN_10 - 8) * 4));
    GPIOA->AFR[1] |= (6U << ((PIN_10 - 8) * 4));

    RCC->APBENR1 |= RCC_APBENR1_I2C1EN;  // enable I2C1 peripheral clock

    I2C1->CR1 &= ~I2C_CR1_PE;                          // disable I2C1 to configure
    I2C1->CR1 &= ~(I2C_CR1_ANFOFF | I2C_CR1_DNF);       // analog filter on, no digital filter

    I2C1->TIMINGR = 0x1032339C;          // 100 kHz SCL from 12 MHz input clock

    I2C1->CR1 |= I2C_CR1_PE;             // enable I2C1

    I2C1->OAR1 &= ~I2C_OAR1_OA1EN;                      // disable own address 1
    I2C1->OAR1 &= ~(I2C_OAR1_OA1 | I2C_OAR1_OA1MODE);   // clear own address 1

    I2C1->CR1 &= ~(I2C_CR1_SMBHEN | I2C_CR1_SMBDEN);    // disable SMBus modes
    I2C1->CR2 &= ~I2C_CR2_NACK;                         // clear NACK generation

    I2C1->CR2 |= I2C_CR2_AUTOEND;        // auto-generate STOP after NBYTES

    I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;     // enable clock stretching
}

int i2c1_read(uint8_t address, uint8_t *buffer, uint16_t length)
{
    // set slave address, byte count, and read direction
    I2C1->CR2 = (I2C1->CR2 & ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN)) |
                ((address << 1) & I2C_CR2_SADD) |
                ((length << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) |
                I2C_CR2_RD_WRN;

    I2C1->CR2 |= I2C_CR2_START;          // generate START condition

    for (uint16_t i = 0; i < length; i++) {
        uint32_t timeout = 10000;

        while (!(I2C1->ISR & I2C_ISR_RXNE)) {  // wait for received byte
            if (--timeout == 0) {
                return -1;
            }
        }

        buffer[i] = (uint8_t)I2C1->RXDR;       // store received byte
    }

    return 0;
}

int i2c1_write(uint8_t address, const uint8_t *data, uint16_t length)
{
    // set slave address, byte count, write direction
    I2C1->CR2 = (I2C1->CR2 &
                 ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN)) |
                ((address << 1) & I2C_CR2_SADD) |
                ((length << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES);

    I2C1->CR2 |= I2C_CR2_START;          // generate START condition

    for (uint16_t i = 0; i < length; i++) {
        uint32_t timeout = 10000;

        while (!(I2C1->ISR & I2C_ISR_TXIS)) {  // wait for TX buffer ready
            if (--timeout == 0) {
                return -1;
            }
        }

        I2C1->TXDR = data[i];            // load next byte to transmit
    }

    uint32_t timeout = 10000;

    while (!(I2C1->ISR & I2C_ISR_STOPF)) {     // wait for auto-generated STOP
        if (--timeout == 0) {
            return -1;
        }
    }

    I2C1->ICR |= I2C_ICR_STOPCF;         // clear STOP flag

    return 0;
}