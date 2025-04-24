/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 17:52:08 by nakhalil          #+#    #+#             */
/*   Updated: 2025/04/24 13:35:50 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/* ── LIBRARIES ───────────────────────────────────────────────────────────── */
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <ctype.h>
# include <fcntl.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>

/* ── TOKEN TYPES ─────────────────────────────────────────────────────────── */
typedef enum e_tok_type
{
	TOK_WORD,
	TOK_PIPE,
	TOK_REDIR_IN,      // <
	TOK_REDIR_OUT,     // >
	TOK_REDIR_APPEND,  // >>
	TOK_REDIR_HEREDOC  // <<
}	t_tok_type;

/* ── TOKEN ───────────────────────────────────────────────────────────────── */
typedef struct s_token
{
	char			*value;
	t_tok_type		type;
	struct s_token	*next;
}	t_token;

/* ── REDIRECTION ─────────────────────────────────────────────────────────── */
typedef struct s_redir
{
	int				type;
	char			*filename;
	struct s_redir	*next;
}	t_redir;

/* ── COMMAND ─────────────────────────────────────────────────────────────── */
typedef struct s_cmd
{
	char			**argv;
	t_redir			*redir;
	int				pipe_fd[2];
	int				infile;
	int				outfile;
	pid_t			pid;
	struct s_cmd	*next;
}	t_cmd;

/* ── SHELL ───────────────────────────────────────────────────────────────── */
typedef struct s_shell
{
	char	*line;
	t_cmd	*cmds;
	// Add more fields here later (e.g. env list, status)
}	t_shell;

/* ── GLOBAL ──────────────────────────────────────────────────────────────── */
extern t_token	*g_tokens;

/* ── FUNCTION PROTOTYPES ─────────────────────────────────────────────────── */

/* lexer.c */
int		lexer(t_shell *shell);
t_token	*tokenize(char *line);
void	free_token_list(t_token *tokens);

/* parser.c */
int		parser(t_shell *shell);

/* utilities */
void	free_all(t_shell *shell);
void	print_tokens(t_token *t); // for debugging (optional)
void	print_cmds(t_cmd *cmd);   // for debugging (optional)

#endif
