/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 17:55:32 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/21 20:05:59 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_error	ensure_command_capacity(t_data *data, int cmd_idx)
{
	int	new_cap;
	int	i;

	if (cmd_idx < data->cmd_cap)
		return (SUCCESS);
	if (data->cmd_cap > 0)
		new_cap = data->cmd_cap * 2;
	else
		new_cap = 16;
	data->commands = ft_realloc(
		data->commands,
		data->cmd_cap * sizeof *data->commands,
		new_cap * sizeof *data->commands
	);
	i = data->cmd_cap;
	while (i < new_cap)
	{
		data->commands[i] = (t_command){NULL, NULL, 0, 0, 0};
		i++;
	}
	data->cmd_cap = new_cap;
	return (SUCCESS);
}

static t_error	add_redirection(t_command *cmd, t_data *data, int *i)
{
	char	*file;

	if (*i + 1 >= data->token_count
		|| data->tokens[*i + 1].type != WORD)
	{
		print_unexpected_token(data->tokens[*i].type);
		return (ERR_SYNTAX);
	}
	cmd->redirs = ft_realloc(
		cmd->redirs,
		cmd->redir_count * sizeof *cmd->redirs,
		(cmd->redir_count + 1) * sizeof *cmd->redirs
	);
	file = ft_strdup(data->tokens[*i + 1].value);
	if (!file)
		return (ERR_MALLOC);
	cmd->redirs[cmd->redir_count++] = (t_redir){file,
		data->tokens[*i].type};
	*i += 2;
	return (SUCCESS);
}

t_error	parse_tokens(t_data *data)
{
	int	cmd_idx;
	int	i;
	t_error	err;

	free_commands(data);
	cmd_idx = 0;
	i = 0;
	while (i < data->token_count)
	{
		err = ensure_command_capacity(data, cmd_idx);
		if (err != SUCCESS)
			return (err);
		data->commands[cmd_idx] = (t_command){NULL, NULL, 0, 0, 0};
		while (i < data->token_count && data->tokens[i].type != PIPE)
		{
			if (data->tokens[i].type >= REDIR_IN)
				err = add_redirection(&data->commands[cmd_idx],
					data, &i);
			else
			{
				data->commands[cmd_idx].args = ft_extend_arr(
					data->commands[cmd_idx].args,
					data->tokens[i++].value
				);
				err = SUCCESS;
			}
			if (err != SUCCESS)
				return (err);
		}
		if (i < data->token_count && data->tokens[i].type == PIPE)
			i++;
		cmd_idx++;
	}
	data->cmd_count = cmd_idx;
	return (SUCCESS);
}
