#ifndef _NGSH_BUILTIN_H
#define _NGSH_BUILTIN_H

int builtin_cd(int, char **);
int builtin_echo(int, char **);
int builtin_exit(int, char **);
int builtin_export(int, char **);
int builtin_history(int, char **);

typedef int (*builtin_handle) (int, char **);
struct builtin_entry {
    const char *token;
    builtin_handle handle;
};

const struct builtin_entry builtin_table[] = {
    {"cd", builtin_cd},
    {"echo", builtin_echo},
    {"exit", builtin_exit},
    {"export", builtin_export},
    {"history", builtin_history}
};

#endif                          /* builtin.h */
