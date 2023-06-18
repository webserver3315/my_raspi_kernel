// 
// HW의 Basic I/O를 세팅하는데 중점을 둠.
// UART를 통해 I/O는 수행됨.
// 진짜 디버깅하려면 점프케이블 4가닥-USB를 연결해주는 USB to TTL 케이블이 필요함
// 
#include <stddef.h>
#include <stdint.h>

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

// enum 설정
enum
{
    // The GPIO registers base address.
    GPIO_BASE = 0x3F200000, // for raspi2 & 3, 0x20200000 for raspi1

    GPPUD = (GPIO_BASE + 0x94),
    GPPUDCLK0 = (GPIO_BASE + 0x98),

    // The base address for UART.
    UART0_BASE = 0x3F201000, // for raspi2 & 3, 0x20201000 for raspi1

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

// 
// boot.S로부터 진입점
// r0 r1 atags 안쓰여서 버림
// 그외 별거없음
// 
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
    //
    // 컴파일러는 아래 세 변수의 사용을 무시하게 됨
    // 즉, 초기화 및 사용 안해도 경고 안띄움
    //
    (void) r0;
    (void) r1;
    (void) atags;

    uart_init();
    uart_puts("Hello, kernel World!\r\n");

    while (1) {
        // UART 응답받고 UART로 전송하고 반복
        uart_putc(uart_getc());
        uart_putc('\n');
    }
}
