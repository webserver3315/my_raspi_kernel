#include <stddef.h>
#include <stdint.h>

#ifndef UART_H
#define UART_H


// static inline void mmio_write(uint32_t reg, uint32_t data);
// static inline uint32_t mmio_read(uint32_t reg);
// static inline void delay(int32_t count);
// static 함수의 선언은 두 번 되어서는 안된다.
// https://kirkim.github.io/c/2021/04/26/inline_func.html


// reg에 쓴다.
// 절대주소를 써야하는데, 0x20000000 + peripheral base + register offset 넣어야 함
static inline void mmio_write(uint32_t reg, uint32_t data)
{
    *(volatile uint32_t*)reg = data; // volatile: 컴파일러가 최적화하지 말라는 뜻.
}

static inline uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t*)reg;
}

// Loop <delay> times in a way that the compiler won't optimize away
// 그냥 시간낭비하는 함수
// 구체적으로는 이해할 수 없어서, chat gpt에게 물어봄
static inline void delay(int32_t count)
{
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : "=r"(count): [count]"0"(count) : "cc");
}


void uart_init();
void uart_putc(unsigned char c);
unsigned char uart_getc();
void uart_puts(const char* str);

enum
{
    // The GPIO registers base address.

#ifdef MODEL_1
    GPIO_BASE = 0x20200000, 
#else
    GPIO_BASE = 0x3F200000, 
#endif

    GPPUD = (GPIO_BASE + 0x94),
    GPPUDCLK0 = (GPIO_BASE + 0x98),

    // The base address for UART.
#ifdef MODEL_1
    UART0_BASE = 0x20201000, 
#else
    UART0_BASE = 0x3F201000, 
#endif

    // 각 32bit씩 차지함
    UART0_DR     = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR     = (UART0_BASE + 0x18),
    UART0_ILPR   = (UART0_BASE + 0x20),
    UART0_IBRD   = (UART0_BASE + 0x24),
    UART0_FBRD   = (UART0_BASE + 0x28),
    UART0_LCRH   = (UART0_BASE + 0x2C),
    UART0_CR     = (UART0_BASE + 0x30),
    UART0_IFLS   = (UART0_BASE + 0x34),
    UART0_IMSC   = (UART0_BASE + 0x38),
    UART0_RIS    = (UART0_BASE + 0x3C),
    UART0_MIS    = (UART0_BASE + 0x40),
    UART0_ICR    = (UART0_BASE + 0x44),
    UART0_DMACR  = (UART0_BASE + 0x48),
    UART0_ITCR   = (UART0_BASE + 0x80),
    UART0_ITIP   = (UART0_BASE + 0x84),
    UART0_ITOP   = (UART0_BASE + 0x88),
    UART0_TDR    = (UART0_BASE + 0x8C),
};


#endif