#ifndef _NGSH_H
#define _NGSH_H

void add_token(char *);
void free_token();
void free_argv(int, char **);
int build_pipe();
int commit();
void free_envp();

#endif                          /* ngsh.h */
