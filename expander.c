/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 14:25:55 by nakhalil          #+#    #+#             */
/*   Updated: 2025/04/29 19:11:40 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * expand_env_var
 *   Given a variable name (without the leading '$') and an environment array,
 *   searches for NAME=value in env.  Returns a newly-allocated copy of the
 *   value part, or an empty string if not found.
 *
 *   var_name: NUL-terminated variable name to look up
 *   env: the inherited environment (NULL-terminated array of "KEY=VALUE")
 *
 *   Returns: malloc’d string of the variable’s value (or "").
 */
static char *expand_env_var(char *var_name, char **env)
{
    int i = 0;
    int len = ft_strlen(var_name);

    while (env[i])
    {
        if (ft_strncmp(env[i], var_name, len) == 0
            && env[i][len] == '=')
            return ft_strdup(env[i] + len + 1);
        i++;
    }
    return ft_strdup("");
}

/**
 * expand_variable
 *   Called when a '$' is encountered in a WORD token.  Extracts the variable
 *   name (handles "$?" specially), looks up its value, and appends it to *result.
 *
 *   result: pointer to the growing result string (must be NULL or malloc’d)
 *   ptr:    pointer into the original token->value at the '$'
 *   data:   full shell data (for last_status and env)
 *
 *   Returns: pointer just *after* the variable name in the original string.
 */
char *expand_variable(char **result, char *ptr, t_data *data)
{
    char var_name[256];
    int  i = 0;

    ptr++;  // Skip the '$'

    if (*ptr == '?')
        var_name[i++] = *ptr++;
    else
        while (ft_isalnum(*ptr) || *ptr == '_')
            var_name[i++] = *ptr++;
    var_name[i] = '\0';

    /* If it was `$?`, expand to last status; otherwise look up in env. */
    char *expanded = (var_name[0] == '?' )
        ? ft_itoa(data->last_status)
        : expand_env_var(var_name, data->env);

        *result = str_append_str(*result, expanded);
    return ptr;
}

/**
 * expand_tokens
 *   Walks every token in data->tokens[].  For each WORD that’s not SINGLE_QUOTE,
 *   repeatedly calls expand_variable (on any '$') and rebuilds token->value.
 *
 *   data: the shell data structure, with token_count + tokens[] filled by the lexer.
 */
void expand_tokens(t_data *data) {
    for (int i = 0; i < data->token_count; i++) {
        t_token *token = &data->tokens[i];

        /* skip non-WORDs and single-quoted WORDs */
        if (token->type != WORD || token->quote == SINGLE_QUOTE)
            continue;

        char *result = ft_strdup("");
        char *ptr = token->value;
        
        while (ptr && *ptr) {
            if (*ptr == '$' && (token->quote != SINGLE_QUOTE)) {
                ptr = expand_variable(&result, ptr, data);
            } else {
                result = str_append_char(result, *ptr);
                ptr++;
            }
        }
        
        free(token->value);
        token->value = result;
    }
}
