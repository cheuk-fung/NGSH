#ifndef _NGSH_H
#define _NGSH_H

void add_token(char *);
void free_token();
int build_pipe();
void cleanup(int, char **);
int commit();
void free_envp();

#endif                          /* ngsh.h */
