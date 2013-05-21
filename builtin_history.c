#include <stdio.h>
#include <string.h>
#include <readline/history.h>
#include <time.h>
#include <unistd.h>

#include "builtin.h"

extern int optind;

int builtin_history(int argc, char *argv[])
{
    static const char *optstring = "ctr";

    int clear = 0;
    int display_time = 0;
    int addition = 1;

    int opt;
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'c':
            clear = 1;
            break;
        case 't':
            display_time = 1;
            break;
        case 'r':
            addition = -1;
            break;
        default:               /* ? */
            goto ERROR;
        }
    }

    if (clear) {
        clear_history();
        goto SUCCESS;
    }

    HIST_ENTRY **history = history_list();
    int i = addition == 1 ? 0 : history_length - 1;
    int last = addition == 1 ? history_length : -1;
    for (; i != last; i += addition) {
        printf("%3d  ", i + 1);
        if (display_time) {
            time_t timestamp = history_get_time(history[i]);
            char timestring[128];
            if (ctime_r(&timestamp, timestring) == NULL) {
                perror(argv[0]);
                goto ERROR;
            }
            int length = strlen(timestring);
            timestring[length - 1] = '\0';      /* remove trailing '\n' */
            printf("%s  ", timestring);
        }
        printf("%s\n", history[i]->line);
    }

  SUCCESS:
    optind = 1;
    return 0;

  ERROR:
    optind = 1;
    return -1;
}
