#include <string.h>

#include "builtin.h"

const struct builtin_entry builtin_table[] = {
    {"cd", builtin_cd},
    {"echo", builtin_echo},
    {"exit", builtin_exit},
    {"export", builtin_export},
    {"history", builtin_history},
    {NULL, NULL}
};

builtin_handle get_builtin(char *cmd)
{
    int i;
    for (i = 0; builtin_table[i].cmd; i++) {
        if (strcmp(cmd, builtin_table[i].cmd) == 0) {
            return builtin_table[i].handle;
        }
    }
    return NULL;
}
