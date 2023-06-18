#include <kernel/mem.h>
#include <kernel/atag.h>
#include <common/stdlib.h>
#include <stdint.h>
#include <stddef.h>

// 링커 스크립트의 __end 주소를 그대로 가져올 수 있다.
extern uint8_t __end;

static uint32_t num_pages;

DEFINE_LIST(page); // 페이지 리스트 정의
IMPLEMENT_LIST(page); // 리스트에 필요한 함수 정의

static page_t * all_pages_array;
page_list_t free_pages;

void mem_init(atag_t * atags) {
    uint32_t mem_size,  page_array_len, kernel_pages, i;

    // Get the total number of pages
    mem_size = get_mem_size(atags); // must return 1024^3
    num_pages = mem_size / PAGE_SIZE; // must return 1024^3/(4*1024)

    // Allocate space for all those pages' metadata.  Start this block just after the kernel image is finished
    page_array_len = sizeof(page_t) * num_pages; // page_array의 길이 걍 계산한 것
    all_pages_array = (page_t *)&__end; // all_pages_array의 첫 주소를 linker script에서 가져올 수 있다.
    // page array는 __end부터, page_array_len Byte만큼 연속적으로 이어져있다.
    bzero(all_pages_array, page_array_len); // 싹다 0으로 밀어버림
    INITIALIZE_LIST(free_pages); // free_pages 리스트 초기화(별건 아니고, head, tail, size 0으로 set)

    // Iterate over all pages and mark them with the appropriate flags
    // Start with kernel pages
    kernel_pages = ((uint32_t)&__end) / PAGE_SIZE; // 0x00~__end 주소까지의 갯수가 kernel page로 할당
    for (i = 0; i < kernel_pages; i++) {
        all_pages_array[i].vaddr_mapped = i * PAGE_SIZE; // 가상주소는 일단 페이지 순번 * 4 * 1024로
        all_pages_array[i].flags.allocated = 1; // 이미 할당 완료
        all_pages_array[i].flags.kernel_page = 1; // 커널페이지 플래그 ON
    }
    // Map the rest of the pages as unallocated, and add them to the free list
    for(; i < num_pages; i++){
        all_pages_array[i].flags.allocated = 0;
        append_page_list(&free_pages, &all_pages_array[i]); // Free 리스트에 append
    }

}

void * alloc_page(void) {
    page_t * page;
    void * page_mem;


    if (size_page_list(&free_pages) == 0)
        return 0;

    // Get a free page
    page = pop_page_list(&free_pages);
    page->flags.kernel_page = 1;
    page->flags.allocated = 1;

    // Get the address the physical page metadata refers to
    // page 주소에서 all_pages_array 시작주소를 빼고, PAGE_SIZE 곱한다.
    // 예를 들어 page 주소가 0x500이고, all_pages_array가 0x100부터 시작한다면
    // 0x100 * 4096의 가상주소를 리턴하게 된다.
    // 솔직히 아리까리한데, 좀더 파고들어봐야할듯
    page_mem = (void *)((page - all_pages_array) * PAGE_SIZE);

    // Zero out the page, big security flaw to not do this :)
    bzero(page_mem, PAGE_SIZE);

    return page_mem;
}

void free_page(void * ptr) {
    page_t * page;

    // Get page metadata from the physical address
    // 앞의 alloc_page의 역산임
    page = all_pages_array + ((uint32_t)ptr / PAGE_SIZE);

    // Mark the page as free
    page->flags.allocated = 0;
    append_page_list(&free_pages, page);
}
