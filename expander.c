/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 14:25:55 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/17 18:43:02 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stddef.h>

/**
 * get_env_value
 *   Find NAME=value in envp; returns pointer to value or NULL.
 */
char *get_env_value(char *name, char **env)
{
    size_t len = ft_strlen(name);

    for (int i = 0; env[i]; i++)
    {
        if (!ft_strncmp(env[i], name, len) && env[i][len] == '=')
            return (env[i] + len + 1);
    }
    return (NULL);
}

/**
 * expand_tokens
 *   Expand environment variables in each unquoted WORD token.
 */
t_error expand_tokens(t_data *data)
{
    for (int t = 0; t < data->token_count; t++)
    {
        t_token *tok = &data->tokens[t];
        if (tok->type != WORD || tok->quote == SINGLE_QUOTE)
            continue;

        /* initialize buffer */
        size_t cap = EXPAND_INIT_CAP;
        size_t len = 0;
        char *buf = safe_malloc(cap);
        buf[0] = '\0';

        char *src = tok->value;
        char quote = 0;

        for (size_t i = 0; src[i]; )
        {
            /* track quote state */
            if (!quote && (src[i] == '"' || src[i] == '\''))
            {
                quote = src[i++];
                continue;
            }
            else if (quote && src[i] == quote)
            {
                quote = 0;
                i++;
                continue;
            }

            /* variable expansion or literal '$' */
            if (src[i] == '$' && quote != '\'')
            {
                /* lookahead */
                char next = src[i + 1];

                /* literal '$' if no special var */
                if (next == '\0'
                 || (next != '?'
                  && !ft_isalnum(next)
                  && next != '_'))
                {
                    /* append '$' literally */
                    if (len + 1 >= cap)
                    {
                        buf = ft_realloc(buf, cap, cap * 2);
                        cap *= 2;
                    }
                    buf[len++] = src[i++];
                    continue;
                }

                /* skip '$' */
                i++;

                /* special $? */
                if (src[i] == '?')
                {
                    char *num = ft_itoa(data->last_status);
                    if(!num)
                    {
                        free(buf);
                        return ERR_MALLOC;
                    }
                    for (size_t j = 0; num[j]; j++)
                    {
                        if (len + 1 >= cap)
                        {
                            buf = ft_realloc(buf, cap, cap * 2);
                            cap *= 2;
                        }
                        buf[len++] = num[j];
                    }
                    free(num);
                    i++;
                    continue;
                }

                /* identifier var: gather its name */
                size_t start = i;
                while (src[i] && (ft_isalnum(src[i]) || src[i] == '_'))
                    i++;
                char *name = ft_substr(src, start, i - start);
                if (!name)
                {
                    free(buf);
                    return (ERR_MALLOC);
                }
                char *val  = get_env_value(name, data->env);
                free(name);

                /* append its value if any */
                if (val)
                {
                    for (size_t j = 0; val[j]; j++)
                    {
                        if (len + 1 >= cap)
                        {
                            buf = ft_realloc(buf, cap, cap * 2);
                            cap *= 2;
                        }
                        buf[len++] = val[j];
                    }
                }
                continue;
            }

            /* normal character copy */
            if (len + 1 >= cap)
            {
                buf = ft_realloc(buf, cap, cap * 2);
                cap *= 2;
            }
            buf[len++] = src[i++];
        }

        /* NUL terminate and replace */
        buf[len] = '\0';
        free(tok->value);
        tok->value = buf;
    }
    return SUCCESS;
}

