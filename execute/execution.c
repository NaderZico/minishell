#include "minishell.h"

/*
 * apply_redirections
 *   - Loop over cmd->redirs[], open FDs, dup2() to STDIN/STDOUT, close originals.
 */
void apply_redirections(t_command *cmd)
{
    // TODO: for each cmd->redirs[i]:
    //   if REDIR_IN  → open(cmd->redirs[i].file, O_RDONLY)
    // ...
}

/*
 * apply_pipes
 *   - Given index i and pipes[][] from the parent,
 *     dup2 read end to STDIN and/or write end to STDOUT.
 */
void apply_pipes(int i, int pipes[][2], int cmd_count)
{
    // TODO
}

/*
 * close_all_pipes
 *   - Parent/child should close all pipe FDs after dup2.
 */
void close_all_pipes(int pipes[][2], int cmd_count)
{
    // TODO
}

/*
 * launch_pipeline
 *   - Fork each command, call apply_redirections & apply_pipes in child,
 *     then either run builtin or exec_external.
 */
void launch_pipeline(t_data *data)
{
    // TODO
}
