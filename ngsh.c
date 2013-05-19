#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <unistd.h>

#include "lex.yy.h"

#define PROMPT_SIZE 128
#define TOKEN_SIZE 128
#define ENVP_SIZE 1024

const char NGSH[] = "ngsh";

int token_count;
char *token[TOKEN_SIZE];
char *envp[ENVP_SIZE];

int prev_fildes[2], post_fildes[2];

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

void add_token(char *buf)
{
    token[token_count++] = strdup(buf);
}

void free_token()
{
    int i;
    for (i = 0; i < token_count; i++) {
        free(token[i]);
        token[i] = NULL;
    }
    token_count = 0;
}

int build_pipe()
{
    if (pipe(post_fildes) == -1) {
        perror(NGSH);
        return -1;
    }

    return 0;
}

static int execvpe(const char *file, char *argv[], char *envp[])
{
    char **saved_environ = environ;
    environ = envp;
    int r = execvp(file, argv);
    environ = saved_environ;
    return r;
}

int commit()
{
    int redirect_stdin = 0, redirect_stdout = 0;
    char *rstdin, *rstdout;

    int argc = 0;
    char **argv = (char **) malloc(sizeof(char *) * token_count);

    int i;
    for (i = 0; i < token_count; i++) {
        if (strcmp(token[i], "<") == 0) {
            redirect_stdin = 1;
            rstdin = token[++i];
        } else if (strcmp(token[i], ">") == 0) {
            redirect_stdout = 1;
            rstdout = token[++i];
        } else {
            argv[argc++] = strdup(token[i]);
        }
    }
    argv[argc] = NULL;

    if ((redirect_stdin && rstdin == NULL)
        || (redirect_stdout && rstdout == NULL)) {
        fprintf(stderr, "NGSH: Parse error\n");
        return EXIT_FAILURE;
    }

    char *cmd = strdup(argv[0]);
    pid_t pid = fork();
    if (pid == -1) {
        perror(NGSH);
        return EXIT_FAILURE;
    } else if (pid == 0) {
        if (prev_fildes[0] != -1) {
            close(prev_fildes[1]);
            dup2(prev_fildes[0], STDIN_FILENO);
            close(prev_fildes[0]);
        }
        if (post_fildes[1] != -1) {
            close(post_fildes[0]);
            dup2(post_fildes[1], STDOUT_FILENO);
            close(post_fildes[1]);
        }

        if (redirect_stdin) {
            FILE *fin;
            if ((fin = fopen(rstdin, "r")) == NULL) {
                perror(NGSH);
                _exit(EXIT_FAILURE);
            }
            dup2(fileno(fin), STDIN_FILENO);
            fclose(fin);
        }
        if (redirect_stdout) {
            FILE *fout;
            if ((fout = fopen(rstdout, "w")) == NULL) {
                perror(NGSH);
                _exit(EXIT_FAILURE);
            }
            dup2(fileno(fout), STDOUT_FILENO);
            fclose(fout);
        }

        execvpe(cmd, argv, envp);
        // The exec() functions return only if an error has occurred. //
        fprintf(stderr, "NGSH: Command not found: %s\n", cmd);
        _exit(EXIT_FAILURE);
    }

    wait(NULL);

    if (post_fildes[1] != -1) {
        prev_fildes[0] = post_fildes[0];
        prev_fildes[1] = post_fildes[1];

        close(post_fildes[1]);
    }

    free(cmd);
    for (i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);

    return 0;
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
        prev_fildes[0] = prev_fildes[1] = -1;
        post_fildes[0] = post_fildes[1] = -1;

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
