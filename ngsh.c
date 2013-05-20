#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin.h"
#include "lex.yy.h"

#define PROMPT_SIZE 128
#define TOKEN_SIZE 128
#define ENVP_SIZE 1024

const char NGSH[] = "ngsh";

int token_count;
char *token[TOKEN_SIZE];
char *cmd_argv[TOKEN_SIZE];
char *envp[ENVP_SIZE];

int saved_fildes[2], pipe_fildes[2];

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

void free_envp()
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
        /* FIXME: Uncomment the following line causes weird behavior of
         * getopt, but keep it commented out will cause a memory leak. */
        // free(token[i]);
        token[i] = NULL;
    }
    token_count = 0;
}

int build_pipe()
{
    if (pipe(pipe_fildes) == -1) {
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
    char *rstdin_file, *rstdout_file;

    int argc = 0;

    int i;
    for (i = 0; i < token_count; i++) {
        if (strcmp(token[i], "<") == 0) {
            redirect_stdin = 1;
            rstdin_file = token[++i];
        } else if (strcmp(token[i], ">") == 0) {
            redirect_stdout = 1;
            rstdout_file = token[++i];
        } else {
            cmd_argv[argc++] = token[i];
        }
    }
    cmd_argv[argc] = (char *) NULL;

    if ((redirect_stdin && rstdin_file == NULL)
        || (redirect_stdout && rstdout_file == NULL)) {
        fprintf(stderr, "%s: Parse error\n", NGSH);
        return -1;
    }

    char *cmd = cmd_argv[0];
    builtin_handle handle = get_builtin(cmd);
    if (handle && saved_fildes[0] == -1 && pipe_fildes[0] == -1
        && !redirect_stdin && !redirect_stdout) {
        /* A built-in command without any pipes or redirects should be
         * executed directly. */
        handle(argc, cmd_argv);
    } else {
        pid_t pid = fork();
        if (pid == -1) {
            perror(NGSH);
            return -1;
        } else if (pid == 0) {
            if (saved_fildes[0] != -1) {
                if (dup2(saved_fildes[0], STDIN_FILENO) == -1) {
                    perror(NGSH);
                    _exit(EXIT_FAILURE);
                }
                if (close(saved_fildes[0]) == -1) {
                    perror(NGSH);
                    _exit(EXIT_FAILURE);
                }
            }
            if (pipe_fildes[1] != -1) {
                if (dup2(pipe_fildes[1], STDOUT_FILENO) == -1) {
                    perror(NGSH);
                    _exit(EXIT_FAILURE);
                }
                if (close(pipe_fildes[1]) == -1) {
                    perror(NGSH);
                    _exit(EXIT_FAILURE);
                }
            }

            if (redirect_stdin) {
                FILE *fin;
                if ((fin = fopen(rstdin_file, "r")) == NULL) {
                    perror(NGSH);
                    _exit(EXIT_FAILURE);
                }
                if (dup2(fileno(fin), STDIN_FILENO) == -1) {
                    perror(NGSH);
                    _exit(EXIT_FAILURE);
                }
                if (fclose(fin) == EOF) {
                    perror(NGSH);
                    _exit(EXIT_FAILURE);
                }
            }
            if (redirect_stdout) {
                FILE *fout;
                if ((fout = fopen(rstdout_file, "w")) == NULL) {
                    perror(NGSH);
                    _exit(EXIT_FAILURE);
                }
                if (dup2(fileno(fout), STDOUT_FILENO) == -1) {
                    perror(NGSH);
                    _exit(EXIT_FAILURE);
                }
                if (fclose(fout) == EOF) {
                    perror(NGSH);
                    _exit(EXIT_FAILURE);
                }
            }

            if (handle) {
                if (handle(argc, cmd_argv) == -1) {
                    _exit(EXIT_FAILURE);
                }
                _exit(EXIT_SUCCESS);
            } else {
                execvpe(cmd, cmd_argv, envp);
                // The exec() functions return only if an error has occurred. //
                fprintf(stderr, "%s: %s: Command not found\n", NGSH, cmd);
                _exit(EXIT_FAILURE);
            }
        }

        if (wait(NULL) == (pid_t) - 1) {
            perror(NGSH);
        }
    }

    if (pipe_fildes[0] != -1) {
        saved_fildes[0] = pipe_fildes[0];
        saved_fildes[1] = pipe_fildes[1];

        if (close(pipe_fildes[1]) == -1) {
            puts("pipe_fildes[1]");
            perror(NGSH);
        }

        pipe_fildes[0] = pipe_fildes[1] = -1;
    }

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
        saved_fildes[0] = saved_fildes[1] = -1;
        pipe_fildes[0] = pipe_fildes[1] = -1;

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

        if (saved_fildes[0] != -1) {
            if (close(saved_fildes[0]) == -1) {
                perror(NGSH);
            }
        }

        free(line);
    }
    printf("\n");

    builtin_exit(0, NULL);

    return 0;
}
