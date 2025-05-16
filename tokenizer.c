/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 15:12:33 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/16 15:43:59 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*──────────────────────────────────────────────────────────────────────────*/
/*                            Helper Functions                             */
/*──────────────────────────────────────────────────────────────────────────*/

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

static int	ensure_token_capacity(t_data *data)
{
	if (data->token_count >= data->token_cap)
	{
		int			new_cap = data->token_cap ? data->token_cap * 2 : 16;
		t_token		*new_tokens;

		new_tokens = safe_malloc(sizeof(t_token) * new_cap);
		if (data->tokens)
		{
			ft_memcpy(new_tokens, data->tokens,
			          sizeof(t_token) * data->token_count);
			free(data->tokens);
		}
		data->tokens    = new_tokens;
		data->token_cap = new_cap;
	}
	return (1);
}

/*──────────────────────────────────────────────────────────────────────────*/
/*                         Token-handling Functions                        */
/*──────────────────────────────────────────────────────────────────────────*/

// 1) Quotes: consume from opening ' or " up to matching close.
static void	handle_quotes(char *input, int *i, t_data *data)
{
	t_quote	quote_type = (input[*i] == '\'') ? SINGLE_QUOTE : DOUBLE_QUOTE;
	int		start      = ++(*i);

	while (input[*i] && input[*i] != quote_type_to_char(quote_type))
		(*i)++;
	if (!input[*i])
	{
		ft_putstr_fd("minishell: unterminated quote\n", 2);
		data->syntax_error = 1;
		return;
	}
	(*i)++; // skip closing quote

	if (!ensure_token_capacity(data))
		return;
	data->tokens[data->token_count++] = (t_token){
		.value = ft_substr(input, start, *i - start - 1),
		.type  = WORD,
		.quote = quote_type
	};
}

static void	handle_word(char *input, int *i, t_data *data)
{
	int		start = *i;
	t_quote	quote = NO_QUOTE;
	char	*value;

	while (input[*i] && ((quote != NO_QUOTE) || 
	(!ft_isspace(input[*i]) && !ft_strchr("|<>", input[*i]))))
{
	if ((input[*i] == '\'' || input[*i] == '"') && quote == NO_QUOTE)
		quote = update_quote_state(quote, input[*i]);
	else if (input[*i] == quote_type_to_char(quote))
		quote = update_quote_state(quote, input[*i]);
	(*i)++;
}
	value = ft_substr(input, start, *i - start);
	if (!value || !ensure_token_capacity(data))
	{
		free(value);
		return;
	}
	data->tokens[data->token_count++] = (t_token){value, WORD, NO_QUOTE};
}

// 3) Pipes: a single '|' becomes a PIPE token.
static void	handle_pipe(char *input, int *i, t_data *data)
{
	(void)input;  // unused
	if (!ensure_token_capacity(data))
		return;
	data->tokens[data->token_count++] = (t_token){
		.value = ft_strdup("|"),
		.type  = PIPE,
		.quote = NO_QUOTE
	};
	(*i)++;
}

// 4) Redirections: <, <<, >, >> become the proper REDIR_* token.
static void	handle_redirection(char *input, int *i, t_data *data)
{
	t_token_type	type;

	if (input[*i] == '<')
	{
		(*i)++;
		if (input[*i] == '<')
		{
			type = REDIR_HEREDOC;
			(*i)++;
		}
		else
			type = REDIR_IN;
	}
	else // '>'
	{
		(*i)++;
		if (input[*i] == '>')
		{
			type = REDIR_APPEND;
			(*i)++;
		}
		else
			type = REDIR_OUT;
	}

	if (!ensure_token_capacity(data))
		return;
	data->tokens[data->token_count++] = (t_token){
		.value = NULL,
		.type  = type,
		.quote = NO_QUOTE
	};
}

/*──────────────────────────────────────────────────────────────────────────*/
/*                         Main Tokenizer Entry                           */
/*──────────────────────────────────────────────────────────────────────────*/

int	tokenize_input(char *input, t_data *data)
{
	int	i;

	// reset
	free(data->tokens);
	data->tokens    = NULL;
	data->token_cap = data->token_count = 0;

	i = 0;
	while (input[i])
	{
		if (ft_isspace(input[i]))
			i++;
		else if (input[i] == '\'' || input[i] == '"')
			handle_quotes(input, &i, data);
		else if (input[i] == '|')
			handle_pipe(input, &i, data);
		else if (input[i] == '<' || input[i] == '>')
			handle_redirection(input, &i, data);
		else
			handle_word(input, &i, data);
	}
	return (0);
}
