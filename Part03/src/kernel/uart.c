// 
// HW의 Basic I/O를 세팅하는데 중점을 둠.
// UART를 통해 I/O는 수행됨.
// 진짜 디버깅하려면 점프케이블 4가닥-USB를 연결해주는 USB to TTL 케이블이 필요함
// 
#include <stddef.h>
#include <stdint.h>

#include <kernel/uart.h>
#include <common/stdlib.h>

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


//
// Setting up the Hardware
// 자세한 내용은 https://jsandler18.github.io/explanations/kernel_c.html 참조
// UART Reference를 읽어봐야 이해할 수 있음.
//
void uart_init()
{
    mmio_write(UART0_CR, 0x00000000);

    mmio_write(GPPUD, 0x00000000);
    delay(150);

    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);

    mmio_write(GPPUDCLK0, 0x00000000);

    mmio_write(UART0_ICR, 0x7FF);

    // 보레이트 설정
    mmio_write(UART0_IBRD, 1);
    mmio_write(UART0_FBRD, 40);

    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
            (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));

    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}


void uart_putc(unsigned char c)
{
    while ( mmio_read(UART0_FR) & (1 << 5) ) { } // FIFO가 써질 준비가 되어있으면
    mmio_write(UART0_DR, c); // 쓴다.
}

unsigned char uart_getc()
{
    while ( mmio_read(UART0_FR) & (1 << 4) ) { } // FIFO가 읽힐 준비가 되어있으면
    return mmio_read(UART0_DR); // 읽는다
}


// 문자열버전(당연)
void uart_puts(const char* str)
{
    for (size_t i = 0; str[i] != '\0'; i ++)
        uart_putc((unsigned char)str[i]);
}

/********* 추가됨 ********/
// uart_flags_t read_flags(void) {
//     uart_flags_t flags;
//     flags.as_int = mmio_read(UART0_FR);
//     return flags;
// }