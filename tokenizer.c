/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 15:12:33 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/12 14:20:46 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * ft_isspace
 *   Returns true if c is any standard ASCII whitespace.
 */
static int ft_isspace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n'
         || c == '\v' || c == '\f' || c == '\r');
}

/**
 * quote_type_to_char
 *   Converts our t_quote enum into its literal delimiter (\' or \").
 */
static char quote_type_to_char(t_quote type)
{
    if (type == SINGLE_QUOTE) return '\'';
    if (type == DOUBLE_QUOTE) return '"';
    return 0;
}

/**
 * update_quote_state
 *   Toggles or maintains quote state when seeing ' or ".
 *   Ensures that nested/unmatched quotes are tracked.
 */
static t_quote update_quote_state(t_quote current, char c)
{
    if (c == '\'')
    {
        if (current == SINGLE_QUOTE)
            return NO_QUOTE;
        else
            return SINGLE_QUOTE;
    }
    else if (c == '"')
    {
        if (current == DOUBLE_QUOTE)
            return NO_QUOTE;
        else
            return DOUBLE_QUOTE;
    }
    else
        return current;
}

/**
 * handle_quotes
 *   Called when lexer sees ' or ".  Grabs everything up to the matching
 *   delimiter (or errors out on unterminated), produces one WORD token
 *   with quote=SINGLE_QUOTE or DOUBLE_QUOTE.
 */
static void	handle_quotes(char *input, int *i, t_data *data)
{
	t_quote	quote_type;
	int		start;
	char	*value;

	if (input[*i] == '\'')
		quote_type = SINGLE_QUOTE;
	else
		quote_type = DOUBLE_QUOTE;
	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != quote_type_to_char(quote_type))
		(*i)++;
	value = ft_substr(input, start, *i - start);
	if (input[*i])
		(*i)++;
	else
	{
		ft_putstr_fd("minishell: unterminated quote\n", 2);
		free(value);
		return ;
	}
	data->tokens[data->token_count].value = value;
	data->tokens[data->token_count].type = WORD;
	data->tokens[data->token_count].quote = quote_type;
	data->token_count++;
}

/**
 * handle_redir_pipe
 *   Called when lexer sees one of |, <, <<, >, >>.
 *   Emits a token of type PIPE/REDIR_* with NULL value.
 */
static void handle_redir_pipe(char *input, int *i, t_data *data)
{
    t_token_type type;

    if (input[*i] == '|')
        type = PIPE, (*i)++;
    else if (input[*i] == '<' && input[*i + 1] == '<')
        type = REDIR_HEREDOC, (*i) += 2;
    else if (input[*i] == '<')
        type = REDIR_IN, (*i)++;
    else if (input[*i] == '>' && input[*i + 1] == '>')
        type = REDIR_APPEND, (*i) += 2;
    else /* must be single > */
        type = REDIR_OUT, (*i)++;

    data->tokens[data->token_count++] = (t_token){NULL, type, NO_QUOTE};
}

/**
 * handle_word
 *   Grabs an unquoted “word” (including embedded quotes) up until whitespace
 *   or one of the special chars.  Emits a WORD token with quote=NO_QUOTE.
 */
static void handle_word(char *input, int *i, t_data *data)
{
    int start = *i;
    t_quote current_quote = NO_QUOTE;

    while (input[*i]
       && (current_quote != NO_QUOTE
        || !ft_strchr(" |<>\"\'", input[*i])))
    {
        if (input[*i] == '\'' || input[*i] == '"')
            current_quote = update_quote_state(current_quote, input[*i]);
        (*i)++;
    }

    char *value = ft_substr(input, start, *i - start);
    data->tokens[data->token_count++] = (t_token){
        .value = value,
        .type  = WORD,
        .quote = NO_QUOTE
    };
}

/**
 * tokenize_input
 *   Top‐level tokenizer: splits `input` into tokens[] up to MAX_TOKENS.
 *   Returns 0 on success (we don’t use the return code for errors here).
 */
int tokenize_input(char *input, t_data *data)
{
    int i = 0;
    data->token_count = 0;

    while (input[i] && data->token_count < MAX_TOKENS - 1)
    {
        while (ft_isspace(input[i])) i++;
        if (!input[i]) break;

        if (input[i] == '|' || input[i] == '<' || input[i] == '>')
            handle_redir_pipe(input, &i, data);
        else if (input[i] == '\'' || input[i] == '"')
            handle_quotes(input, &i, data);
        else
            handle_word(input, &i, data);
    }
    return 0;
}
