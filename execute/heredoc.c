#include "minishell.h"

/*
 * handle_heredocs
 *   Iterate all commands’ redirs[], for each REDIR_HEREDOC:
 *     • create pipe
 *     • read lines until delimiter
 *     • write to pipe[1], close write end
 *     • store pipe[0] in a new cmd->heredoc_fd field
 */
void handle_heredocs(t_data *data)
{
    // TODO
}
