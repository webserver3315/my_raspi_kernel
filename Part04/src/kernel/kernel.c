#include <stdint.h>
#include <kernel/uart.h>

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
    //
    // 컴파일러는 아래 세 변수의 사용을 무시하게 됨
    // 즉, 초기화 및 사용 안해도 경고 안띄움
    //
    (void) r0;
    (void) r1;
    (void) atags;

    char buf[256];

    uart_init();
    puts("Hello, kernel World!\n");

    while (1) {
        // Echo 동작을 반복
        gets(buf, 256);
        puts(buf);
        putc('\n');        
    }
}
