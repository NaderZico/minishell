/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 17:52:08 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/16 15:49:44 by nakhalil         ###   ########.fr       */
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

typedef struct s_data {
    t_token         *tokens;
    int             token_count;
    int             token_cap;
    t_command       *commands;
    int             cmd_count;
    int             cmd_cap;
    char            **env;
    int             last_status;
    int             syntax_error; // <-- Add this line
}                   t_data;

int					tokenize_input(char *input, t_data *data);
int					parse_tokens(t_data *data);
int					validate_syntax(t_data *data);
void				expand_tokens(t_data *data);
void				free_data(t_data *data);
void				*safe_malloc(size_t size);
char				*str_append_char(char *s, char c);
char				*str_append_str(char *s, char *suffix);
char				**ft_extend_arr(char **arr, char *new_str);
void				ft_free_arr(char **arr);
void				print_unexpected_token(t_token_type type);
int					check_redir_sequence(t_data *data, int *i);
int					check_pipe_sequence(t_data *data, int *i, int *prev_was_word);
void				setup_signals(void);

#endif