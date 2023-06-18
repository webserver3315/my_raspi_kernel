@
@ linker가 참고한다. 이 코드가 compiled binary에서 어디에 있는지를 뜻한다.
@ .section 지시어는 ARM 어셈블리어가 다음 명령어가 위치할 섹션을 지정하는데 사용된다.
@ 즉, 이 명령어는 .text.boot 섹션에 배치될 것이다는 뜻이다.
@ 
.section ".text.boot"

@
@ linker가 참고하게 된다.
@ .global 지시어는 어셈블리에서 심볼이 전역접근가능하다는 선언이다.
@ _start 라벨은 특수한 라벨이다.
@ _start 라벨이 프로그램의 실행 시작점을 표시하는게 국룰이다.
@ 
.global _start

_start:
    @ 4개 코어 중 3개를 halt 시킨다.    
    mrc p15, #0, r1, c0, c0, #5
    and r1, r1, #3
    cmp r1, #0
    bne halt

    @ 스택포인터를 0x8000에서 시작시킨다.
    mov sp, #0x8000

    @ 
    @ BSS Section의 물리주소를 register로 로드
    @ BSS는 C에서 초기화안된 전역변수가 저장되는 곳.
    @ 자세한건 학부 메모리영역 파트 참조
    @ 
    @ 아무튼, 이 두 symbol은 이따 linker랑 놀 때 정의될 것.
    @  
    ldr r4, =__bss_start
    ldr r9, =__bss_end

    @ 레지스터 4개를 초기화
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    @ 2로 branch
    @ f는 forward에 있다는 음어?같은거임. 필수는아님.
    b       2f

1:
    @
    @ 루프물
    @ 2에서 되돌아올수도있음
    @ op2의 값(0x00)을 [op1]으로 넣음
    @ 즉, bss를 전부 0으로 초기화함
    @ 
    stmia r4!, {r5-r8}

2:
    @ r4의 0과 r9의 BSS_End 값이 같은지 확인
    cmp r4, r9
    @ r4가 r9보다 작거나 같으면 1로 브랜치
    blo 1b

    @ 여기왔다는건 bss가 전부 0으로 초기화되었다는 것
    @ kernel_main이라는 C함수의 주소를 load해옴
    ldr r3, =kernel_main
    @ 점프
    blx r3

halt:
    wfe
    @ C함수가 리턴되면 진입
    @ 영원히 아무것도 하지 않음
    b halt