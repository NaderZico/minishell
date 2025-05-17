/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 15:12:33 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/17 20:24:42 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
	     || c == '\v' || c == '\f' || c == '\r');
}

static char	quote_type_to_char(t_quote type)
{
	if (type == SINGLE_QUOTE)
		return ('\'');
	if (type == DOUBLE_QUOTE)
		return ('"');
	return (0);
}

static t_quote	update_quote_state(t_quote current, char c)
{
	if (c == '\'')
		return (current == SINGLE_QUOTE ? NO_QUOTE : SINGLE_QUOTE);
	else if (c == '"')
		return (current == DOUBLE_QUOTE ? NO_QUOTE : DOUBLE_QUOTE);
	return (current);
}

static t_error ensure_token_capacity(t_data *data)
{
    int new_cap = data->token_cap ? data->token_cap * 2 : 16;

    if (data->token_count >= data->token_cap)
    {
        // grow the tokens[] array with ft_realloc (exits on failure)
        data->tokens = ft_realloc(
            data->tokens,
            data->token_cap * sizeof *data->tokens,
            new_cap * sizeof *data->tokens
        );
        data->token_cap = new_cap;
    }
    return (SUCCESS);
}

static t_error add_token(t_data *data, char *value, t_token_type type, t_quote quote)
{
    ensure_token_capacity(data); // No error check needed; safe_malloc exits on failure

    // Handle operator token values
    if (type != WORD && !value)
    {
        if (type == PIPE)
            value = ft_strdup("|");
        else if (type == REDIR_IN)
            value = ft_strdup("<");
        else if (type == REDIR_OUT)
            value = ft_strdup(">");
        else if (type == REDIR_APPEND)
            value = ft_strdup(">>");
        else if (type == REDIR_HEREDOC)
            value = ft_strdup("<<");
        
        if (!value)
            return (ERR_MALLOC);
    }

    data->tokens[data->token_count++] = (t_token){value, type, quote};
    return (SUCCESS);
}

static t_error handle_quotes(char *input, int *i, t_data *data)
{
    t_quote quote_type = (input[*i] == '\'') ? SINGLE_QUOTE : DOUBLE_QUOTE;
    int start = ++(*i);

    while (input[*i] && input[*i] != quote_type_to_char(quote_type))
        (*i)++;
    if (!input[*i])
    {
        ft_putstr_fd("minishell: unterminated quote\n", 2);
        data->syntax_error = 1;
        return (ERR_SYNTAX);
    }
    char *value = ft_substr(input, start, *i - start);
    if (!value)
        return (ERR_MALLOC);
    (*i)++;
    return add_token(data, value, WORD, quote_type);
}

static t_error handle_word(char *input, int *i, t_data *data)
{
    int start = *i;
    t_quote quote = NO_QUOTE;
    char *value;

    while (input[*i] && ( 
        (quote != NO_QUOTE) || 
        (!ft_isspace(input[*i]) && !ft_strchr("|<>", input[*i]))
    ))
    {
        if ((input[*i] == '\'' || input[*i] == '"') && quote == NO_QUOTE)
            quote = update_quote_state(quote, input[*i]);
        else if (input[*i] == quote_type_to_char(quote))
            quote = update_quote_state(quote, input[*i]);
        (*i)++;
    }
    value = ft_substr(input, start, *i - start);
    if (!value)
        return (ERR_MALLOC);
    return add_token(data, value, WORD, NO_QUOTE);
}

static t_error handle_pipe(char *input, int *i, t_data *data)
{
    (void)input;
    char *pipe_str = ft_strdup("|");
    if (!pipe_str)
        return (ERR_MALLOC);
    t_error err = add_token(data, pipe_str, PIPE, NO_QUOTE);
    if (err != SUCCESS)
        free(pipe_str);
    (*i)++;
    return err;
}

static t_error handle_redirection(char *input, int *i, t_data *data)
{
    t_token_type type;

    if (input[*i] == '<')
    {
        (*i)++;
        type = (input[*i] == '<') ? (++(*i), REDIR_HEREDOC) : REDIR_IN;
    }
    else
    {
        (*i)++;
        type = (input[*i] == '>') ? (++(*i), REDIR_APPEND) : REDIR_OUT;
    }
    return add_token(data, NULL, type, NO_QUOTE);
}

t_error tokenize_input(char *input, t_data *data)
{
    int i = 0;
    t_error err = SUCCESS;

    free_tokens(data);
    while (input[i] && err == SUCCESS)
    {
        if (ft_isspace(input[i]))
            i++;
        else if (input[i] == '\'' || input[i] == '"')
            err = handle_quotes(input, &i, data);
        else if (input[i] == '|')
            err = handle_pipe(input, &i, data);
        else if (input[i] == '<' || input[i] == '>')
            err = handle_redirection(input, &i, data);
        else
            err = handle_word(input, &i, data);
    }
    return (err == SUCCESS && data->syntax_error) ? ERR_SYNTAX : err;
}