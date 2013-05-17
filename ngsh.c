#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <unistd.h>

#define PROMPT_SIZE 128
#define ARGV_SIZE 128
#define ENVP_SIZE 1024

char *cmd_argv[ARGV_SIZE];
char *cmd_envp[ENVP_SIZE];

int load_envp(char *envp[])
{
    int envc = 0;
    for (; envp[envc] != NULL; envc++) {
        cmd_envp[envc] = strdup(envp[envc]);
    }
    return envc;
}

void free_envp()
{
    int envc = 0;
    for (; cmd_envp[envc] != NULL; envc++) {
        free(cmd_envp[envc]);
    }
}

char *get_prompt()
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

int get_argv(char *cmdline)
{
    int argc = 0;
    char *curr = cmdline;
    while (*curr != '\0') {
        char *next = curr;
        while (!isblank(*next) && *next != '\0') {
            next++;
        }
        cmd_argv[argc++] = strndup(curr, next - curr);
        while (isblank(*next)) {
            next++;
        }
        curr = next;
    }
    return argc;
}

void free_argv(int argc)
{
    int i;
    for (i = 0; i < argc; i++) {
        free(cmd_argv[i]);
        cmd_argv[i] = 0;
    }
}

int main(int argc, char *argv[], char *envp[])
{
    load_envp(envp);

    while (1) {
        char *prompt = get_prompt();
        char *cmdline = readline(prompt);
        free(prompt);
        if (cmdline == NULL) {
            break;
        }

        add_history(cmdline);

        int cmd_argc = get_argv(cmdline);
        char *cmd = strdup(cmd_argv[0]);

        pid_t pid = fork();
        if (pid == 0) {
            execve(cmd, cmd_argv, cmd_envp);
            /* The exec() functions return only if an error has occurred. */
            printf("NGSH> command not found: %s\n", cmd);
            _exit(EXIT_FAILURE);
        } else {
            wait(NULL);
        }

        free(cmdline);
        free(cmd);
        free_argv(cmd_argc);
    }
    printf("\n");

    free_envp();

    return 0;
}
