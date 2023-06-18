# Makefile은 indent를 tab으로만 받음에 유의

# 컴파일러 변수 선언 - 경로로 해도 됨
CC = arm-none-eabi-gcc

RASPI_MODEL = 3
ifeq ($(RASPI_MODEL),1) # 실행안됨
	CPU = arm1176jzf-s
	DIRECTIVES = -D MODEL_1
# 만약 RASPI_MODEL == 1 에 종속적 코드가 있다면,
# 사용시 #ifdef MODEL_1 ~~~ #endif 식으로 감싸서 대응할 수 있게 됨.
else
	CPU = cortex-a7 # 이게 실행됨
endif

# Makefile: $(변수명)을 통해 커맨드에 변수를 대입할 수 있다.
CFLAGS= -mcpu=$(CPU) -fpic -ffreestanding $(DIRECTIVES) # 컴파일할 때 쓰이는 옵션들
CSRCFLAGS= -O2 -Wall -Wextra # C소스 컴파일 시 쓰였던 옵션들
LFLAGS= -ffreestanding -O2 -nostdlib # 링크할때 쓰였던 옵션들

# 상대경로 지정(start with Makefile directory)
KER_SRC = ../src/kernel
COMMON_SRC = ../src/common
KER_HEAD = ../include

# 모든 binary object 를 저장할 폴더를 지정. 만약, 없으면 알아서 만드니 걱정 ㄴㄴ
OBJ_DIR = objects

# 커널소스파일의 리스트
KERSOURCES = $(wildcard $(KER_SRC)/*.c)
# 참고) wildcard == 패턴에 해당되는 모든 파일을 열거해주는 makefile 커맨드
# 즉, 위의 명령어는 "../src/kernel의 *.c 패턴의 모든 파일을 열거해서 리스트형식으로 KERSOURCES에 할당하라".

# 마찬가지로, 커널 커먼소스파일의 리스트
COMMONSOURCES = $(wildcard $(COMMON_SRC)/*.c)
ASMSOURCES = $(wildcard $(KER_SRC)/*.S)

# $(patsubst pattern, replacement, text) 함수
# text의 특정 패턴을 교체한다.
OBJECTS = $(patsubst $(KER_SRC)/%.c, $(OBJ_DIR)/%.o, $(KERSOURCES))
OBJECTS += $(patsubst $(COMMON_SRC)/%.c, $(OBJ_DIR)/%.o, $(COMMONSOURCES))
OBJECTS += $(patsubst $(KER_SRC)/%.S, $(OBJ_DIR)/%.o, $(ASMSOURCES))
# 즉 모든 윗 경로 파일들의 확장자명만 .o로 바꾼 것들이 OBJECTS에 싹 포함되어있다.
# ../src/kernel/file.c => objects/file.o 가 되는 식.

HEADERS = $(wildcard $(KER_HEAD)/*.h)

# 커널 바이너리 이름
IMG_NAME=kernel.img

print:
	echo $(KERSOURCES $(KERSOURCES))

# 커널 최종 build에는 object파일과 헤더파일이 필요함
build: $(OBJECTS) $(HEADERS)
	echo $(OBJECTS)
	$(CC) -T linker.ld -o $(IMG_NAME) $(LFLAGS) $(OBJECTS)

# .o 파일을 얻고 싶다면, 동일한 prefix를 가진 KER_SRC/.c 파일이 있어야 함을 의미한다.
# %는 any nonempty string이다.
$(OBJ_DIR)/%.o: $(KER_SRC)/%.c
# p옵션: 자동으로 하위디렉토리까지 생성
# $(@) : dir/foo.o라면
# $(@D): dir로 끝남.
	mkdir -p $(@D)
# I옵션: source file이 #include <../../include/kernel/header.h> 대신 #include <kernel/header.h> 로 접근되게 해줌
# $<: first dependency => 즉 .c 파일
# $@: target => 즉 .o 파일
	$(CC) $(CFLAGS) -I$(KER_SRC) -I$(KER_HEAD) -c $< -o $@ $(CSRCFLAGS)

# .S로부터도 .o 만들기
$(OBJ_DIR)/%.o: $(KER_SRC)/%.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(KER_SRC) -c $< -o $@

# common의 .c로부터 .o 만들기
$(OBJ_DIR)/%.o: $(COMMON_SRC)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(KER_SRC) -I$(KER_HEAD) -c $< -o $@ $(CSRCFLAGS)

clean:
	rm -rf $(OBJ_DIR)
	rm $(IMG_NAME)

run: build
	qemu-system-aarch64 -m 1024 -M raspi2b -serial stdio -kernel kernel.img

# target: dependencies
#     command
# $@는 target
# $^는 모든 dependencies
# $?는 변경된 dependencies
# $<는 첫 dependencies