/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_check.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 14:32:31 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/17 18:55:49 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_error check_pipes(t_data *data)
{
    int i;
    int prev_was_word;

    if (data->token_count == 0)
        return (SUCCESS);
    if (data->tokens[0].type == PIPE
        || data->tokens[data->token_count - 1].type == PIPE)
    {
        print_unexpected_token(PIPE);
        return (ERR_SYNTAX);
    }
    prev_was_word = 1;
    i = 1;
    return (check_pipe_sequence(data, &i, &prev_was_word) ? SUCCESS : ERR_SYNTAX);
}

static t_error check_redirs(t_data *data)
{
    int i;

    if (data->token_count == 0)
        return (SUCCESS);
    if (data->tokens[0].type >= REDIR_IN
        && data->tokens[0].type <= REDIR_HEREDOC)
    {
        print_unexpected_token(data->tokens[0].type);
        return (ERR_SYNTAX);
    }
    i = 0;
    return (check_redir_sequence(data, &i) ? SUCCESS : ERR_SYNTAX);
}

static t_error check_delimiter_sequences(t_data *data)
{
    int i = 1;

    while (i < data->token_count)
    {
        if (data->tokens[i].type != WORD
            && data->tokens[i - 1].type != WORD)
        {
            print_unexpected_token(data->tokens[i].type);
            return (ERR_SYNTAX);
        }
        i++;
    }
    return (SUCCESS);
}

t_error validate_syntax(t_data *data)
{
    if (data->token_count == 1 && data->tokens[0].type == WORD
        && data->tokens[0].value[0] == '\0')
    {
        handle_error(ERR_CMD_NOT_FOUND, data, NULL);
        return (ERR_CMD_NOT_FOUND);
    }

    t_error err;
    if ((err = check_pipes(data)) != SUCCESS
        || (err = check_redirs(data)) != SUCCESS
        || (err = check_delimiter_sequences(data)) != SUCCESS)
    {
        data->last_status = err; // Directly use the error code
        return (err);
    }
    return (SUCCESS);
}
