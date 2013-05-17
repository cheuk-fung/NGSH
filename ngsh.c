#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

char *get_prompt(char *prompt, size_t prompt_size)
{
    char hostname[128];
    gethostname(hostname, sizeof hostname);

    char *pwd = getenv("PWD");
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

    snprintf(prompt, prompt_size, "%s@%s:%s> ", getenv("USER"), hostname,
             pwd);

    return prompt;
}

int main(int argc, char *argv[])
{
    while (1) {
        char prompt[128];
        get_prompt(prompt, sizeof prompt);

        char *cmdline = readline(prompt);
        if (cmdline == NULL) {
            break;
        }

        add_history(cmdline);

        free(cmdline);
    }
    printf("\n");

    return 0;
}
