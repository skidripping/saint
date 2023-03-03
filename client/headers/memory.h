#pragma once

typedef struct _memblock {
    size_t size;
    struct _memblock* next;
    char data[1];
} memblock;

memblock* head = NULL;

size_t mem_malloc_usable_size(void* ptr);
void* mem_malloc(size_t size);
void* mem_calloc(size_t nmemb, size_t size);
void* mem_realloc(void* ptr, size_t size);
void mem_free(void* ptr);
