#ifndef _NGSH_H
#define _NGSH_H

void add_token(char *);
void free_token();
int build_pipe();
int commit();

extern const char *NGSH;

#endif                          /* ngsh.h */
