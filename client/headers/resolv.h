#pragma once

#include "includes.h"

struct resolv_entries {
    uint8_t addrs_len;
    ipv4_t *addrs;
};

void resolv_domain_to_hostname(char *dst_hostname, char *src_domain, int domain_len);
struct resolv_entries *resolv_lookup(char *);
void resolv_entries_free(struct resolv_entries *);
