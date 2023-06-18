/*
/* 이것은 linker script이다.
/* . 은 current address를 의미한다.(리눅스와 뜻 자체는 같네?)
/* [학부 상식]
/* C에서, .text는 실행가능한 코드
/* C에서, .rodata는 읽기전용 데이터(글로벌상수)
/* C에서, .data는 컴파일시 초기화되는 글로벌변수
/* C에서, .bss는 컴파일시 안초기화되는 글로벌변수
/*
*/

/* boot.S의 _start 심볼이 우리 코드의 entry point가 된다는 선언*/
ENTRY(_start)
 
SECTIONS
{
    /* Starts at LOADER_ADDR. */
    . = 0x8000;
    /* __start와 __text_start 심볼을 0x0800으로.*/
    __start = .;
    __text_start = .;
    .text :
    {
        /* 첫 빠따는 .text.boot 섹션. 즉, boot.S에서 정의된 그 섹션*/
        KEEP(*(.text.boot)) /* KEEP은 linker가 .text.boot 안의 코드를 최적화하지 않게 해줌

        /* 그 이후로는, 순서관계없이 다른 모든개채의 모든 .text 섹션을 순서 무관하게 append함*/
        *(.text)
    }
    . = ALIGN(4096); /* align to page size */
    __text_end = .; /* __text_end는 4096의 배수로 맞춰진상태로 저장됨.*/
 
    /* 이하반복*/
    __rodata_start = .;
    .rodata :
    {
        *(.rodata)
    }
    . = ALIGN(4096); /* align to page size */
    __rodata_end = .;
 
    /* 이하반복*/
    __data_start = .;
    .data :
    {
        *(.data)
    }
    . = ALIGN(4096); /* align to page size */
    __data_end = .;
 
    /* 이하반복*/
    __bss_start = .;
    .bss :
    {
        bss = .;
        *(.bss)
    }
    . = ALIGN(4096); /* align to page size */
    __bss_end = .;
    __end = .;
}
