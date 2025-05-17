/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 17:52:08 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/17 19:50:29 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stddef.h>
# include <stdlib.h>
# include <unistd.h>

extern volatile sig_atomic_t g_signal;

/* Maximum initial buffer for expansion */
# define EXPAND_INIT_CAP 64

typedef enum e_quote
{
	NO_QUOTE,
	SINGLE_QUOTE,
	DOUBLE_QUOTE
}					t_quote;

typedef enum e_token_type
{
	WORD,
	PIPE,
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}					t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	t_quote			quote;
}					t_token;

typedef struct s_redir
{
	char			*file;
	t_token_type	type;
}					t_redir;

typedef struct s_command
{
	char			**args;
	t_redir			*redirs;
	int				redir_count;
	int				pipe_in;
	int				pipe_out;
}					t_command;


typedef struct s_data
{
	t_token			*tokens;
	int				token_count;
	int				token_cap;
	t_command		*commands;
	int				cmd_count;
	int				cmd_cap;
	char			**env;
	int				last_status;
	int				syntax_error;
	char		*input;
	volatile sig_atomic_t	signal_status; 
}					t_data;

typedef enum e_error
{
	SUCCESS = 0,
	ERR_SYNTAX = 2, // Bash-compatible syntax error
	ERR_CMD_NOT_FOUND = 127,
	ERR_SIGINT = 130,  // Ctrl-C exit code
	ERR_SIGQUIT = 131, // Ctrl-\ exit code
	ERR_MALLOC = 255   // Internal error (not POSIX)
	// Add others as needed (e.g., file not found, permission denied)
}					t_error;

/* core */
t_error				tokenize_input(char *input, t_data *data);
t_error				parse_tokens(t_data *data);
t_error				validate_syntax(t_data *data);
t_error				expand_tokens(t_data *data);
void				setup_signals(void);

/* utils */
void				*safe_malloc(size_t size);
void				*ft_realloc(void *ptr, size_t old_size, size_t new_size);
char				**ft_extend_arr(char **arr, char *new_str);
void				append_char(char **buf, size_t *cap, size_t *len, char c);

/* expander helpers */
char	*get_env_value(char *name, char **env); // searches envp[]

/* syntax check */
void				print_unexpected_token(t_token_type type);
t_error				check_redir_sequence(t_data *data, int *i);
t_error				check_pipe_sequence(t_data *data, int *i,
						int *prev_was_word);

/* readline integration */
int					rl_replace_line(const char *text, int clear_undo);
void				rl_redisplay(void);

void				free_commands(t_data *data);
void				free_tokens(t_data *data);
void				free_data(t_data *data);
void				ft_free_arr(char **arr);
void				handle_error(t_error err, t_data *data, char *context);

#endif
