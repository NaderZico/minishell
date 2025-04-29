#include "minishell.h"

/*
 * is_builtin
 *   Return 1 if `cmd` matches one of: echo, cd, pwd, export, unset, env, exit
 */
int is_builtin(const char *cmd)
{
    // TODO
}

/*
 * run_builtin
 *   Execute the builtin in-process (parent or child depending on pipeline length).
 *   Update *last_status accordingly.
 */
void run_builtin(t_command *cmd, int *last_status)
{
    // TODO: dispatch to:
    //   builtin_echo(cmd->args, last_status);
    //   builtin_cd(...);
    //   etc.
}
