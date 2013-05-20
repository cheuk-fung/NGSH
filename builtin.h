#ifndef _NGSH_BUILTIN_H
#define _NGSH_BUILTIN_H

typedef int (*builtin_handle) (int, char **);
struct builtin_entry {
    const char *cmd;
    builtin_handle handle;
};

int builtin_cd(int, char **);
int builtin_echo(int, char **);
int builtin_exit(int, char **);
int builtin_export(int, char **);
int builtin_history(int, char **);
builtin_handle get_builtin(char *);

#endif                          /* builtin.h */
