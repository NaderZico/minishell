/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_free.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 15:23:54 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/17 19:28:09 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void free_tokens(t_data *data)
{
    if (data->tokens)
    {
        for (int i = 0; i < data->token_count; i++)
            free(data->tokens[i].value);
        free(data->tokens);
    }
    data->tokens = NULL;
    data->token_count = data->token_cap = 0;
}

void free_commands(t_data *data)
{
    if (data->commands)
    {
        for (int c = 0; c < data->cmd_count; c++)
        {
            ft_free_arr(data->commands[c].args);
            for (int r = 0; r < data->commands[c].redir_count; r++)
                free(data->commands[c].redirs[r].file);
            free(data->commands[c].redirs);
        }
        free(data->commands);
    }
    data->commands = NULL;
    data->cmd_count = data->cmd_cap = 0;
}

void free_data(t_data *data)
{
    free_tokens(data);
    free_commands(data);
}

void    ft_free_arr(char **arr)
{
    int i = 0;
    if (!arr)
        return;
    while (arr[i])
        free(arr[i++]);
    free(arr);
}
