/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 17:55:32 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/17 19:33:11 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_error ensure_command_capacity(t_data *data, int cmd_idx)
{
    int new_cap = data->cmd_cap ? data->cmd_cap * 2 : 16;

    if (cmd_idx >= data->cmd_cap)
    {
        data->commands = ft_realloc(
            data->commands,
            data->cmd_cap * sizeof(t_command),
            new_cap * sizeof(t_command)
        );
        for (int i = data->cmd_cap; i < new_cap; i++)
            data->commands[i] = (t_command){NULL, NULL, 0, 0, 0};
        data->cmd_cap = new_cap;
    }
    return (SUCCESS);
}

static t_error	add_redirection(t_command *cmd, t_data *data, int *i)
{
    if (*i + 1 >= data->token_count || data->tokens[*i + 1].type != WORD)
    {
        print_unexpected_token(data->tokens[*i].type);
        return (ERR_SYNTAX);
    }

    cmd->redirs = ft_realloc(
        cmd->redirs,
        cmd->redir_count * sizeof(t_redir),
        (cmd->redir_count + 1) * sizeof(t_redir)
    );
	char *file = ft_strdup(data->tokens[*i + 1].value);
    if (!file)
        return (ERR_MALLOC);
    cmd->redirs[cmd->redir_count] = (t_redir){.file = file, .type = data->tokens[*i].type};
    cmd->redir_count++;
    *i += 2;
    return (SUCCESS);
}

t_error parse_tokens(t_data *data)
{
    int cmd_idx = 0;
    int i = 0;

    free_commands(data);

    while (i < data->token_count)
    {
        if (ensure_command_capacity(data, cmd_idx) != SUCCESS)
            return (ERR_MALLOC);
        data->commands[cmd_idx] = (t_command){NULL, NULL, 0, 0, 0};

        while (i < data->token_count && data->tokens[i].type != PIPE)
        {
            if (data->tokens[i].type >= REDIR_IN)
            {
                t_error err = add_redirection(&data->commands[cmd_idx], data, &i);
                if (err != SUCCESS)
                    return (err);
            }
            else
            {
                data->commands[cmd_idx].args = ft_extend_arr(
                    data->commands[cmd_idx].args,
                    data->tokens[i++].value
                );
                if (!data->commands[cmd_idx].args)
                    return (ERR_MALLOC);
            }
        }
        if (i < data->token_count && data->tokens[i].type == PIPE)
            i++;
        cmd_idx++;
    }
    data->cmd_count = cmd_idx;
    return (SUCCESS);
}
