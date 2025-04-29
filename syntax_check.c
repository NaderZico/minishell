/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_check.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 14:32:31 by nakhalil          #+#    #+#             */
/*   Updated: 2025/04/29 18:24:53 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Print a more specific “unexpected token” error.
 */
static void print_unexpected_token(t_token_type type)
{
    if (type == PIPE)
        ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
    else if (type == REDIR_IN)
        ft_putstr_fd("minishell: syntax error near unexpected token `<'\n", 2);
    else if (type == REDIR_OUT)
        ft_putstr_fd("minishell: syntax error near unexpected token `>'\n", 2);
    else if (type == REDIR_HEREDOC)
        ft_putstr_fd("minishell: syntax error near unexpected token `<<'\n", 2);
    else if (type == REDIR_APPEND)
        ft_putstr_fd("minishell: syntax error near unexpected token `>>'\n", 2);
}

/**
 * Disallow:
 *   - first or last token being a pipe
 *   - two pipes in a row
 *   - pipe with no command on either side
 */
static int check_pipes(t_data *data)
{
    int i;
    int prev_was_word;

    if (data->token_count == 0)
        return (1);

    if (data->tokens[0].type == PIPE)
    {
        print_unexpected_token(PIPE);
        return (0);
    }
    if (data->tokens[data->token_count - 1].type == PIPE)
    {
        print_unexpected_token(PIPE);
        return (0);
    }

    prev_was_word = 1;  // treat “before the first token” as a command boundary
    for (i = 1; i < data->token_count; i++)
    {
        if (data->tokens[i].type == PIPE)
        {
            if (!prev_was_word
                || data->tokens[i + 1].type == PIPE)
            {
                print_unexpected_token(PIPE);
                return (0);
            }
            prev_was_word = 0;
        }
        else
            prev_was_word = (data->tokens[i].type == WORD);
    }
    return (1);
}

/**
 * Disallow:
 *   - redir at start or end
 *   - redir not followed by a WORD
 *   - two redirs in a row
 */
static int check_redirs(t_data *data)
{
    int i;

    if (data->token_count == 0)
        return (1);

    /* 1) cannot start with a redirection */
    if (data->tokens[0].type >= REDIR_IN && data->tokens[0].type <= REDIR_HEREDOC)
    {
        print_unexpected_token(data->tokens[0].type);
        return (0);
    }

    i = 0;
    while (i < data->token_count)
    {
        if (data->tokens[i].type >= REDIR_IN
            && data->tokens[i].type <= REDIR_HEREDOC)
        {
            /* must not be last, and must be followed by WORD */
            if (i + 1 >= data->token_count
                || data->tokens[i + 1].type != WORD)
            {
                print_unexpected_token(data->tokens[i].type);
                return (0);
            }
            /* no two redirs back-to-back */
            if (i > 0
                && data->tokens[i - 1].type >= REDIR_IN
                && data->tokens[i - 1].type <= REDIR_HEREDOC)
            {
                print_unexpected_token(data->tokens[i].type);
                return (0);
            }
            i += 2;  // skip the filename
        }
        else
            i++;
    }
    return (1);
}

/**
 * Disallow any two non-WORD tokens in a row
 * (e.g. “| >”, “<< >>”, etc.)
 */
static int check_delimiter_sequences(t_data *data)
{
    int i;

    i = 1;
    while (i < data->token_count)
    {
        if (data->tokens[i].type != WORD
            && data->tokens[i - 1].type != WORD)
        {
            print_unexpected_token(data->tokens[i].type);
            return (0);
        }
        i++;
    }
    return (1);
}

/**
 * validate_syntax
 *   Top‐level syntax validator.  Combines pipe, redir, and
 *   delimiter‐sequence checks.  On *any* failure, resets data->cmd_count=0
 *   so that free_data() is safe, then returns 0.
 *
 *   Returns: 1 if syntax is OK, 0 on error.
 */
int validate_syntax(t_data *data)
{
    if (!check_pipes(data)
        || !check_redirs(data)
        || !check_delimiter_sequences(data))
    {
        data->cmd_count = 0;
        return (0);
    }
    return (1);
}
