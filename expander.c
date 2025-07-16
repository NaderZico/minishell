/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 14:25:55 by nakhalil          #+#    #+#             */
/*   Updated: 2025/07/16 12:14:00 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stddef.h>

char	*get_env_value(char *name, char **env)
{
	size_t	len;
	int		i;

	len = ft_strlen(name);
	i = 0;
	while (env[i])
	{
		if (!ft_strncmp(env[i], name, len) && env[i][len] == '=')
			return (env[i] + len + 1);
		i++;
	}
	return (NULL);
}

static void	append_str(char **buf, size_t *cap, size_t *len, const char *str)
{
	size_t	i;

	i = 0;
	while (str && str[i])
		append_char(buf, cap, len, str[i++]);
}

static int	handle_dollar(char **buf, size_t *cap, size_t *len, char *src,
		size_t *j, t_data *data)
{
	char	next;
	size_t	start;
	char	*name;
	char	*val;
	char	*num;

	next = src[*j + 1];
	if (next == '\0' || (next != '?' && !ft_isalnum(next) && next != '_'))
	{
		append_char(buf, cap, len, src[(*j)++]);
		return (0);
	}
	(*j)++;
	if (src[*j] == '?')
	{
		num = ft_itoa(data->last_status);
		if (!num)
			return (-1);
		append_str(buf, cap, len, num);
		free(num);
		(*j)++;
		return (0);
	}
	start = *j;
	while (src[*j] && (ft_isalnum(src[*j]) || src[*j] == '_'))
		(*j)++;
	name = ft_substr(src, start, *j - start);
	if (!name)
		return (-1);
	val = get_env_value(name, data->env);
	append_str(buf, cap, len, val);
	free(name);
	return (0);
}

static int	expand_word_token(t_token *token, t_data *data)
{
	size_t	cap = EXPAND_INIT_CAP, len = 0, j;
	char	*buf = safe_malloc(cap), *src = token->value, quote;

	cap = EXPAND_INIT_CAP, len = 0, j = 0;
	buf = safe_malloc(cap), src = token->value, quote = 0;
	buf[0] = '\0';
	while (src[j])
	{
		if (!quote && (src[j] == '"' || src[j] == '\''))
		{
			quote = src[j++];
			continue ;
		}
		if (quote && src[j] == quote)
		{
			quote = 0;
			j++;
			continue ;
		}
		if (src[j] == '$' && quote != '\'')
		{
			if (handle_dollar(&buf, &cap, &len, src, &j, data) == -1)
			{
				free(buf);
				return (-1);
			}
			continue ;
		}
		append_char(&buf, &cap, &len, src[j++]);
	}
	buf[len] = '\0';
	free(token->value);
	token->value = buf;
	return (0);
}

t_error	expand_tokens(t_data *data)
{
	size_t	i;

	i = 0;
	while (i < (size_t)data->token_count)
	{
		if (data->tokens[i].type == WORD
			&& data->tokens[i].quote != SINGLE_QUOTE)
			if (expand_word_token(&data->tokens[i], data) == -1)
				return (ERR_MALLOC);
		i++;
	}
	return (SUCCESS);
}
