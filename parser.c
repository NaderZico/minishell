/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 17:55:32 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/08 14:14:13 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Adds a redirection to the current command.
 *
 * This function reallocates the command's redirection array to accommodate
 * a new redirection, copies existing redirections into the new array,
 * appends the new redirection (based on the current token and the next one),
 * and updates the command's redirection count.
 */
static int	add_redirection(t_command *cmd, t_data *data, int *i)
{
	t_redir	*new_redirs;
	int		j;

	// Allocate space for one more redirection.
	new_redirs = safe_malloc(sizeof(t_redir) * (cmd->redir_count + 1));

	// Copy existing redirections into the new array.
	j = -1;
	while (++j < cmd->redir_count)
		new_redirs[j] = cmd->redirs[j];

	// Add the new redirection (type and target file).
	new_redirs[cmd->redir_count].type = data->tokens[*i].type;
	new_redirs[cmd->redir_count].file = ft_strdup(data->tokens[*i + 1].value);

	// Replace the old redirection array with the new one.
	free(cmd->redirs);
	cmd->redirs = new_redirs;
	cmd->redir_count++;

	// Skip over redirection token and its target.
	*i += 2;
	return (0);
}

/**
 * Parses the list of tokens into an array of command structures.
 *
 * This function reads the tokens, splits them into commands (based on PIPE tokens),
 * adds redirections when detected, and collects arguments.
 *
 * All parsed commands and their data are stored in `data->commands`,
 * and the total number of parsed commands is stored in `data->cmd_count`.
 */
int	parse_tokens(t_data *data)
{
	int	cmd_idx = 0;
	int	i = 0;

	data->cmd_count = 0;

	while (i < data->token_count && cmd_idx < MAX_COMMANDS)
	{
		data->commands[cmd_idx].args = NULL;
		data->commands[cmd_idx].redirs = NULL;
		data->commands[cmd_idx].redir_count = 0;

		// Parse the tokens for the current command until a PIPE or end is reached.
		while (i < data->token_count && data->tokens[i].type != PIPE)
		{
			// Handle redirection token and target.
			if (data->tokens[i].type >= REDIR_IN)
				add_redirection(&data->commands[cmd_idx], data, &i);
			else
			{
				// Add token to argument list.
				data->commands[cmd_idx].args = ft_extend_arr(
					data->commands[cmd_idx].args, data->tokens[i++].value);
			}
		}

		// Skip the PIPE token if present.
		if (i < data->token_count && data->tokens[i].type == PIPE)
			i++;

		// Move to the next command slot.
		cmd_idx++;
	}

	data->cmd_count = cmd_idx;
	return (0);
}
