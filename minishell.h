/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 17:52:08 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/04 15:27:26 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdlib.h>
# include <unistd.h>

/*
 * Limits
 *   MAX_TOKENS   — maximum number of lexical tokens in one input line
 *   MAX_COMMANDS — maximum number of pipe-separated commands
 */
# define MAX_TOKENS 100
# define MAX_COMMANDS 20

/*
 * t_quote
 *   Tracks whether a token was enclosed in single or double quotes,
 *   or not quoted at all.  Single quotes inhibit *all* expansion,
 *   double quotes inhibit everything except `$`‐expansion.
 */
typedef enum e_quote
{
	NO_QUOTE,
	SINGLE_QUOTE,
	DOUBLE_QUOTE
}					t_quote;

/*
 * t_token_type
 *   Classifies each lexed item as:
 *     WORD         — a bare word (possibly with quotes)
 *     PIPE         — '|' for piping
 *     REDIR_IN     — '<' for input redirection
 *     REDIR_OUT    — '>' for output redirection (truncate)
 *     REDIR_APPEND — '>>' for output append
 *     REDIR_HEREDOC— '<<' here‐document
 */
typedef enum e_token_type
{
	WORD,
	PIPE,
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}					t_token_type;

/*
 * s_token
 *   Holds the raw text and metadata of each lexed piece.
 *     value — malloc’d string (for REDIR/PIPE, value is NULL)
 *     type  — one of t_token_type
 *     quote — whether it came from within single/double quotes
 */
typedef struct s_token
{
	char			*value;
	t_token_type	type;
	t_quote			quote;
}					t_token;

/*
 * s_redir
 *   Represents a single redirection on a command:
 *     file — filename or here‐doc delimiter string
 *     type — what kind of redirection this is
 */
typedef struct s_redir
{
	char			*file;
	t_token_type	type;
}					t_redir;

/*
 * s_command
 *   Encapsulates one command (one segment of a possible pipeline):
 *     args        — NULL‐terminated argv array (after expansion)
 *     redirs      — dynamic array of s_redir entries
 *     redir_count — how many entries in redirs[]
 *     pipe_in     — fd to use for STDIN (set later by executor)
 *     pipe_out    — fd to use for STDOUT
 */
typedef struct s_command
{
	char			**args;
	t_redir			*redirs;
	int				redir_count;
	int				pipe_in;
	int				pipe_out;
}					t_command;

/*
 * s_data
 *   Top‐level “session” state, passed around during one REPL iteration:
 *     tokens      — tokenized tokens[MAX_TOKENS]
 *     token_count — actual number of tokens
 *     commands    — parsed commands[MAX_COMMANDS]
 *     cmd_count   — actual number of commands
 *     env         — inherited environment pointer
 *     last_status — exit status of last foreground job ($?)
 */
typedef struct s_data
{
	t_token			tokens[MAX_TOKENS];
	int				token_count;
	t_command		commands[MAX_COMMANDS];
	int				cmd_count;
	char			**env;
	int				last_status;
}					t_data;

/* === Lexer (lexer.c) === */
int					tokenize_input(char *input, t_data *data);

/* === Parser (parser.c + syntax_check.c) === */
int					parse_tokens(t_data *data);
int					validate_syntax(t_data *data);

/* === Expander (expander.c) === */
void				expand_tokens(t_data *data);

/* === Utilities (utils.c) === */
void				free_data(t_data *data);
void				*safe_malloc(size_t size);
char				*str_append_char(char *s, char c);
char				*str_append_str(char *s, char *suffix);
char				**ft_extend_arr(char **arr, char *new_str);
void				ft_free_arr(char **arr);
char				*expand_variable(char **result, char *ptr, t_data *data);

/* === Signals (signals.c) === */
void				setup_signals(void);

#endif
