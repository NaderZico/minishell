#include "minishell.h"

t_token	*g_tokens = NULL;

static t_token	*new_token(char *value, t_tok_type type)
{
	t_token	*tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->value = value;
	tok->type = type;
	tok->next = NULL;
	return (tok);
}

static void	add_token(t_token **tokens, t_token *new)
{
	if (!*tokens)
		*tokens = new;
	else
	{
		t_token *tmp = *tokens;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

static t_token *parse_operator(char *line, int *i)
{
	int			start = *i;
	t_tok_type	type;

	if (line[*i] == '<' && line[*i + 1] == '<')
	{
		*i += 2;
		type = TOK_REDIR_HEREDOC;
	}
	else if (line[*i] == '>' && line[*i + 1] == '>')
	{
		*i += 2;
		type = TOK_REDIR_APPEND;
	}
	else if (line[*i] == '>')
	{
		*i += 1;
		type = TOK_REDIR_OUT;
	}
	else if (line[*i] == '<')
	{
		*i += 1;
		type = TOK_REDIR_IN;
	}
	else // '|'
	{
		*i += 1;
		type = TOK_PIPE;
	}
	return new_token(strndup(&line[start], *i - start), type);
}

static t_token *parse_word(char *line, int *i)
{
	int		start = *i;
	int		j = *i;
	char	quote = 0;

	while (line[j] && !(line[j] == ' ' || line[j] == '\t' || line[j] == '|' || line[j] == '<' || line[j] == '>'))
	{
		if (!quote && (line[j] == '\'' || line[j] == '"'))
		{
			quote = line[j++];
			while (line[j] && line[j] != quote)
				j++;
			if (line[j] == quote)
				j++;
			quote = 0;
		}
		else
			j++;
	}
	*i = j;
	return new_token(strndup(&line[start], j - start), TOK_WORD);
}

t_token	*tokenize(char *line)
{
	t_token *tokens = NULL;
	t_token *new = NULL;
	int i = 0;

	while (line[i])
	{
		while (line[i] && (line[i] == ' ' || line[i] == '\t'))
			i++;

		if (!line[i])
			break;

		if (line[i] == '|' || line[i] == '<' || line[i] == '>')
			new = parse_operator(line, &i);
		else
			new = parse_word(line, &i);

		if (!new)
			return (free_token_list(tokens), NULL);
		add_token(&tokens, new);
	}
	return (tokens);
}

int	lexer(t_shell *shell)
{
	g_tokens = tokenize(shell->line);
	return (g_tokens != NULL);
}
