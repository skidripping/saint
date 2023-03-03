#pragma once

#include "includes.h"

#define CONVERT_ADDR(x) x & 0xff, (x >> 8) & 0xff, (x >> 16) & 0xff, (x >> 24) & 0xff

int util_strlen(const char *str);
int util_strncmp(const char *str1, const char *str2, size_t n);
int util_strcmp(const char *str1, const char *str2);
int util_strcpy(char *dst, const char *src);

int util_atoi(const char *str);
int util_memsearch(const char *buf, size_t buf_len, const char *mem, size_t mem_len);
int *util_stristr(const char *haystack, const char *needle);

char *util_itoa(int num, char *str, int base);
char *util_fdgets(char *buffer, int buffer_size, int fd);

void util_strcat(char *, char *);
void *util_zero(void *buf, size_t len);
void util_memcpy(void *dst, const void *src, size_t n);

int util_isupper(char c);
int util_isalpha(char c);
int util_isspace(char c);
int util_isdigit(char c);

BOOL mem_exists(char *, int, char *, int);

ipv4_t util_local_addr(void);
