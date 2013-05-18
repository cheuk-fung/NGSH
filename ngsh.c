#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <unistd.h>

#include "lex.yy.h"

#define PROMPT_SIZE 128
#define ARGV_SIZE 128
#define ENVP_SIZE 1024

int argc;
char *argv[ARGV_SIZE];
char *envp[ENVP_SIZE];

extern char **environ;
extern char *yylinebuf;

static int load_envp()
{
    int envc = 0;
    for (; environ[envc] != NULL; envc++) {
        envp[envc] = strdup(environ[envc]);
    }
    return envc;
}

static void free_envp()
{
    int envc = 0;
    for (; envp[envc] != NULL; envc++) {
        free(envp[envc]);
    }
}

static int execvpe(const char *file, char *argv[], char *envp[])
{
    char **saved_environ = environ;
    environ = envp;
    int r = execvp(file, argv);
    environ = saved_environ;
    return r;
}

void add_argv(char *buf)
{
    argv[argc++] = strdup(buf);
}

void free_argv()
{
    int i;
    for (i = 0; i < argc; i++) {
        free(argv[i]);
        argv[i] = 0;
    }
    argc = 0;
}


void commit()
{
    char *cmd = strdup(argv[0]);
    pid_t pid = fork();
    if (pid == 0) {
        execvpe(cmd, argv, envp);
        // The exec() functions return only if an error has occurred. //
        printf("NGSH> command not found: %s\n", cmd);
        _exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }
}

static char *set_prompt()
{
    char hostname[128];
    gethostname(hostname, sizeof hostname);

    const char *pwd = getenv("PWD");
    if (strcmp(pwd, getenv("HOME")) == 0) {
        pwd = "~";
    } else if (strcmp(pwd, "/") != 0) {
        /* Get the last directory of pwd. */
        while (*pwd) {
            pwd++;
        }
        while (*(pwd - 1) != '/') {
            pwd--;
        }
    }

    char *prompt = (char *) malloc(sizeof(char) * PROMPT_SIZE);
    snprintf(prompt, PROMPT_SIZE, "%s@%s:%s> ", getenv("USER"), hostname,
             pwd);

    return prompt;
}

int main(int argc, char *argv[])
{
    load_envp();

    while (1) {
        char *prompt = set_prompt();
        char *line = readline(prompt);
        free(prompt);
        if (line == NULL) {
            break;              /* EOF */
        }
        if (line[0] == '\0') {
            continue;           /* Skip blank line. */
        }

        add_history(line);

        strcat(line, "\n");
        yylinebuf = line;
        yylex();

        free(line);
    }
    printf("\n");

    free_envp();

    return 0;
}
