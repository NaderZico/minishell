/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 17:52:08 by nakhalil          #+#    #+#             */
/*   Updated: 2025/04/23 18:01:34 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <ctype.h>
# include <readline/readline.h>
# include <readline/history.h>

extern struct s_token *g_tokens;

typedef enum e_tok_type
{
	TOK_WORD,
	TOK_PIPE,
	TOK_REDIR_IN,      // <
	TOK_REDIR_OUT,     // >
	TOK_REDIR_APPEND,  // >>
	TOK_REDIR_HEREDOC  // <<
}	t_tok_type;

typedef struct s_token
{
	char			*value;
	t_tok_type		type;
	struct s_token	*next;
}	t_token;

typedef struct s_shell
{
	char	*line;
}	t_shell;

// ───────────────────────────────────────────────────────────────
// Lexer function
// ───────────────────────────────────────────────────────────────
int	lexer(t_shell *shell);
void	free_token_list(t_token *tokens);

#endif
