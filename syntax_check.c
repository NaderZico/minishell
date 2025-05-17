/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_check.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 14:32:31 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/17 13:47:46 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_pipes(t_data *data)
{
	int	i;
	int	prev_was_word;

	if (data->token_count == 0)
		return (1);
	if (data->tokens[0].type == PIPE
		|| data->tokens[data->token_count - 1].type == PIPE)
	{
		print_unexpected_token(PIPE);
		return (0);
	}
	prev_was_word = 1;
	i = 1;
	return (check_pipe_sequence(data, &i, &prev_was_word));
}

static int	check_redirs(t_data *data)
{
	int	i;

	if (data->token_count == 0)
		return (1);
	if (data->tokens[0].type >= REDIR_IN
		&& data->tokens[0].type <= REDIR_HEREDOC)
	{
		print_unexpected_token(data->tokens[0].type);
		return (0);
	}
	i = 0;
	return (check_redir_sequence(data, &i));
}

static int	check_delimiter_sequences(t_data *data)
{
	int	i;

	i = 1;
	while (i < data->token_count)
	{
		if (data->tokens[i].type != WORD
			&& data->tokens[i - 1].type != WORD)
		{
			print_unexpected_token(data->tokens[i].type);
			return (0);
		}
		i++;
	}
	return (1);
}

int	validate_syntax(t_data *data)
{
	if (data->token_count == 1 && data->tokens[0].type == WORD
        && data->tokens[0].value[0] == '\0')
    {
        ft_putstr_fd("minishell: command not found\n", 2);
        return (0);
    }

	if (!check_pipes(data)
	 || !check_redirs(data)
	 || !check_delimiter_sequences(data))
	{
		data->cmd_count = 0;
		return (0);
	}
	return (1);
}
