#include <stddef.h>

#include "headers/util.h"
#include "headers/memory.h"

size_t mem_malloc_usable_size(void* ptr) {
    if (ptr == NULL) {
        return 0;
    }
    // get the size of the allocated block by subtracting the size of the
    // metadata structure from the pointer to the block
    return *((size_t*)ptr - 1) - sizeof(size_t);
}

void* mem_malloc(size_t size) {
    memblock* block;
    size_t total_size;

    // align size to multiple of 8
    size = (size + 7) & ~7;

    // search for a free block in the list
    for (block = head; block != NULL; block = block->next) {
        if (block->size >= size) {
            // if found, use the free block
            if (block->size > size + sizeof(memblock)) {
                // split the block if there is enough space for a new block
                memblock* new_block = (memblock*)(block->data + size);
                new_block->size = block->size - size - sizeof(memblock);
                new_block->next = block->next;
                block->size = size;
                block->next = new_block;
            }
            // mark the block as used and return its data
            block->size |= 1;
            return block->data;
        }
    }
    // if no free block is found, allocate a new one using sbrk
    total_size = size + sizeof(memblock);
    block = (memblock*)sbrk(total_size);
    if (block == (void*)-1) {
        return NULL;
    }
    block->size = size | 1;
    block->next = head;
    head = block;
    return block->data;
}

void* mem_calloc(size_t nmemb, size_t size) {
    void* ptr = mem_malloc(nmemb * size);
    if (ptr != NULL) {
        // zero out the allocated memory
        util_zero(ptr, nmemb * size);
    }
    return ptr;
}

void* mem_realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        // if ptr is NULL, treat realloc like malloc
        return mem_malloc(size);
    } else if (size == 0) {
        // if size is 0, treat realloc like free
        mem_free(ptr);
        return NULL;
    } else {
        // allocate a new block of memory of the requested size
        void* new_ptr = mem_malloc(size);
        if (new_ptr == NULL) {
            return NULL;
        }
        // copy the contents of the old block to the new block
        size_t old_size = mem_malloc_usable_size(ptr);
        util_memcpy(new_ptr, ptr, old_size < size ? old_size : size);
        // free the old block
        mem_free(ptr);
        // return the new block
        return new_ptr;
    }
}

void mem_free(void* ptr) {
    memblock* block = (memblock*)((char*)ptr - offsetof(memblock, data));
    block->size &= ~1;
    // merge contiguous free blocks
    while (block->next != NULL && (block->next->size & 1) == 0) {
        block->size += block->next->size + sizeof(memblock);
        block->next = block->next->next;
    }
}
