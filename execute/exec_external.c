#include "minishell.h"

/*
 * exec_external
 *   Search PATH for cmd->args[0], check access(), and execve().
 *   On failure, print perror and set *last_status to 127.
 */
void exec_external(char **args, char **envp, int *last_status)
{
    // TODO
}
