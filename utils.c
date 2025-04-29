/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 13:33:37 by nakhalil          #+#    #+#             */
/*   Updated: 2025/04/29 18:50:20 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * ft_extend_arr
 *   Appends new_str to a NULL-terminated array of strings,
 *   returns the new malloc’d array.
 */
char	**ft_extend_arr(char **arr, char *new_str)
{
	char	**new;
	int		count;
	int		i;

	count = 0;
	while (arr && arr[count])
		count++;
	new = safe_malloc(sizeof(char *) * (count + 2));
	i = -1;
	while (++i < count)
		new[i] = arr[i];
	new[count] = ft_strdup(new_str);
	new[count + 1] = NULL;
	free(arr);
	return (new);
}

/**
 * free_data
 *   Frees all dynamic memory in data->tokens and data->commands,
 *   including token->value, commands[].args[], commands[].redirs[].file.
 */
void	free_data(t_data *data)
{
	int	i;
	int	j;

	i = -1;
	while (++i < data->token_count)
		free(data->tokens[i].value);
	
	i = -1;
	while (++i < data->cmd_count)
	{
		ft_free_arr(data->commands[i].args);
		j = -1;
		while (++j < data->commands[i].redir_count)
			free(data->commands[i].redirs[j].file);
		free(data->commands[i].redirs);
	}
}

/**
 * safe_malloc
 *   malloc wrapper that exits(1) on failure.
 */
void    *safe_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (!ptr)
    {
        write(2, "Malloc error\n", 13);
        exit(EXIT_FAILURE);
    }
    return (ptr);
}

/**
 * str_append_char
 *   Append a single char 'c' to the end of string 's'.
 *   Reallocates a new buffer of length+2, copies s, frees s,
 *   writes c then '\0', and returns the new pointer.
 */
char *str_append_char(char *s, char c)
{
    size_t len = s ? ft_strlen(s) : 0;
    char *new_str = safe_malloc(len + 2);
    if (s)
    {
        ft_memcpy(new_str, s, len);
        free(s);
    }
    new_str[len]     = c;
    new_str[len + 1] = '\0';
    return new_str;
}

/**
 * str_append_str
 *   Append the entire string 'suffix' onto 's'.  Reallocates a new buffer of
 *   length(s)+length(suffix)+1, copies both, frees both inputs, and returns it.
 */
char *str_append_str(char *s, char *suffix)
{
    size_t len1 = s ? ft_strlen(s) : 0;
    size_t len2 = ft_strlen(suffix);
    char *new   = safe_malloc(len1 + len2 + 1);

    if (s)
    {
        ft_memcpy(new, s, len1);
        free(s);
    }
    ft_memcpy(new + len1, suffix, len2 + 1);
    free(suffix);
    return new;
}

/**
 * ft_free_arr
 *   Frees a NULL-terminated array of strings.
 */
void	ft_free_arr(char **arr)
{
	int	i;

	i = 0;
	while (arr && arr[i])
		free(arr[i++]);
	free(arr);
}
