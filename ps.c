#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

extern int optind;

int main(int argc, char *argv[])
{
    const char *PROGNAME = argv[0];

    int all = 0;
    int verbose = 0;

    int opt;
    while ((opt = getopt(argc, argv, "av")) != -1) {
        switch (opt) {
        case 'a':
            all = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        default:               /* ? */
            return -1;
        }
    }

    DIR *dirp;
    if ((dirp = opendir("/proc")) == NULL) {
        perror(PROGNAME);
        return -1;
    }

    if (verbose) {
        printf("%5s %5s %6s %6s %-4s COMMAND\n", "PID", "PPID", "VSZ",
               "RSS", "STAT");
    } else {
        printf("%5s %-4s COMMAND\n", "PID", "STAT");
    }

    pid_t ppid_target = getppid();
    struct dirent *dirent;
    while ((dirent = readdir(dirp))) {
        if (strspn(dirent->d_name, "0123456789") == strlen(dirent->d_name)) {
            char statbuf[256];
            FILE *fstat;
            pid_t pid, ppid;
            char cmd[256], stat;
            int stat_cnt = 0, vsz, rss;

            sprintf(statbuf, "/proc/%s/stat", dirent->d_name);
            if ((fstat = fopen(statbuf, "r")) == NULL) {
                perror(PROGNAME);
                return -1;
            }

            fscanf(fstat, "%d (%[^)]) %c %d", &pid, cmd, &stat, &ppid);
            stat_cnt = 4;
            if (!all && pid != ppid_target && ppid != ppid_target) {
                goto NEXT;
            }

            if (verbose) {
                while (++stat_cnt < 23) {
                    fscanf(fstat, " %*s");
                }
                fscanf(fstat, " %d %d", &vsz, &rss);
                printf("%5d %5d %8d %8d %-4c %s\n", pid, ppid, vsz / 1024,
                       rss * 4, stat, cmd);
            } else {
                printf("%5d %-4c %s\n", pid, stat, cmd);
            }

          NEXT:
            if (fclose(fstat) == EOF) {
                perror(PROGNAME);
                return -1;
            }
        }
    }

    if (closedir(dirp) == -1) {
        perror(PROGNAME);
        return -1;
    }

    return 0;
}
