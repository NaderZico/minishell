/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 17:55:32 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/17 13:16:14 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ensure_command_capacity(t_data *data, int cmd_idx)
{
	if (cmd_idx >= data->cmd_cap)
	{
		int			new_cap = data->cmd_cap ? data->cmd_cap * 2 : 16;
		t_command	*new_commands = safe_malloc(sizeof(t_command) * new_cap);

		if (data->commands)
		{
			ft_memcpy(new_commands,
			          data->commands,
			          sizeof(t_command) * data->cmd_count);
			free(data->commands);
		}
		for (int i = data->cmd_count; i < new_cap; i++)
			new_commands[i] = (t_command){NULL, NULL, 0, 0, 0};
		data->commands = new_commands;
		data->cmd_cap  = new_cap;
	}
	return (1);
}

static int	add_redirection(t_command *cmd, t_data *data, int *i)
{
	if (*i + 1 >= data->token_count
	 || data->tokens[*i + 1].type != WORD)
	{
		print_unexpected_token(data->tokens[*i].type);
		return (1);
	}

	t_redir *new_redirs = safe_malloc(
		sizeof(t_redir) * (cmd->redir_count + 1));
	ft_memcpy(new_redirs,
	          cmd->redirs,
	          sizeof(t_redir) * cmd->redir_count);

	new_redirs[cmd->redir_count].file = 
	    ft_strdup(data->tokens[*i + 1].value);
	new_redirs[cmd->redir_count].type = data->tokens[*i].type;

	free(cmd->redirs);
	cmd->redirs = new_redirs;
	cmd->redir_count++;
	*i += 2;
	return (0);
}

int	parse_tokens(t_data *data)
{
	int	cmd_idx = 0;
	int	i       = 0;

	free(data->commands);
	data->commands = NULL;
	data->cmd_cap  = 0;
	data->cmd_count = 0;

	while (i < data->token_count)
	{
		ensure_command_capacity(data, cmd_idx);
		data->commands[cmd_idx] = (t_command){NULL, NULL, 0, 0, 0};

		while (i < data->token_count && data->tokens[i].type != PIPE)
		{
			if (data->tokens[i].type >= REDIR_IN)
			{
				if (add_redirection(&data->commands[cmd_idx], data, &i))
				{
					free_data(data);
					return (1);
				}
			}
			else
			{
				data->commands[cmd_idx].args =
					ft_extend_arr(
						data->commands[cmd_idx].args,
						data->tokens[i++].value
					);
			}
		}
		if (i < data->token_count && data->tokens[i].type == PIPE)
			i++;
		cmd_idx++;
	}
	data->cmd_count = cmd_idx;
	return (0);
}
