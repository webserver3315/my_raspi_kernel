
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
