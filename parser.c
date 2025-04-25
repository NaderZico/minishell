/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 17:55:32 by nakhalil          #+#    #+#             */
/*   Updated: 2025/04/25 18:24:37 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Allocates and initializes a new command node (t_cmd).
** Sets default values for pipe and redirection fields.
*/
static t_cmd	*new_cmd(void)
{
	t_cmd	*cmd;

	cmd = (t_cmd *)ft_calloc(1, sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->infile = STDIN_FILENO;
	cmd->outfile = STDOUT_FILENO;
	cmd->pipe_fd[0] = -1;
	cmd->pipe_fd[1] = -1;
	cmd->pid = -1;
	cmd->next = NULL;
	return (cmd);
}

/*
** Adds a t_cmd node to the end of the command list.
*/
static void	add_cmd(t_cmd **cmd_list, t_cmd *new)
{
	t_cmd	*tmp;

	if (!*cmd_list)
		*cmd_list = new;
	else
	{
		tmp = *cmd_list;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

/*
** Allocates a redirection node (t_redir) for a given type and filename.
** Returns NULL on allocation failure.
*/
static t_redir	*new_redir(int type, char *filename)
{
	t_redir	*redir;

	redir = (t_redir *)ft_calloc(1, sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->filename = ft_strdup(filename);
	if (!redir->filename)
	{
		free(redir);
		return (NULL);
	}
	redir->next = NULL;
	return (redir);
}

/*
** Appends a redirection node to the end of a command’s redirection list.
*/
static void	add_redir(t_redir **redir_list, t_redir *new)
{
	t_redir	*tmp;

	if (!*redir_list)
		*redir_list = new;
	else
	{
		tmp = *redir_list;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

/*
** Counts the number of arguments (TOK_WORD tokens) before the next pipe.
** Skips redirection operators and filenames.
*/
static int	count_args(t_token *tok)
{
	int	count;

	count = 0;
	while (tok && tok->type != TOK_PIPE)
	{
		if (tok->type == TOK_WORD)
			count++;
		else if (tok->type >= TOK_REDIR_IN && tok->type <= TOK_REDIR_HEREDOC)
			tok = tok->next;
		if (tok)
			tok = tok->next;
	}
	return (count);
}

/*
** Builds the argv array for the current command.
** Only includes TOK_WORD tokens, skipping redirection operators and filenames.
*/
static char	**build_argv(t_token *tok)
{
	int		argc;
	char	**argv;
	int		i;

	argc = count_args(tok);
	argv = (char **)ft_calloc(argc + 1, sizeof(char *));
	if (!argv)
		return (NULL);
	i = 0;
	while (tok && tok->type != TOK_PIPE)
	{
		if (tok->type == TOK_WORD)
			argv[i++] = ft_strdup(tok->value);
		else if (tok->type >= TOK_REDIR_IN && tok->type <= TOK_REDIR_HEREDOC)
			tok = tok->next;
		if (tok)
			tok = tok->next;
	}
	argv[i] = NULL;
	return (argv);
}

/*
** Parses one command segment:
** - Builds argv from TOK_WORD tokens
** - Collects redirections and appends to the command
** - Adds the command to the pipeline list
** - Advances token pointer past the pipe (if present)
*/
static int	parse_command(t_cmd **cmd_list, t_token **tok)
{
	t_cmd	*cmd;
	t_token	*t;
	t_redir	*redir;

	cmd = new_cmd();
	if (!cmd)
		return (0);
	cmd->argv = build_argv(*tok);
	if (!cmd->argv)
	{
		free(cmd);
		return (0);
	}
	t = *tok;
	while (t && t->type != TOK_PIPE)
	{
		if (t->type >= TOK_REDIR_IN && t->type <= TOK_REDIR_HEREDOC)
		{
			int	type = t->type;
			t = t->next;
			if (!t || t->type != TOK_WORD)
			{
				ft_putstr_fd("Syntax error near redirection\n", STDERR_FILENO);
				return (0);
			}
			redir = new_redir(type, t->value);
			if (!redir)
				return (0);
			add_redir(&cmd->redir, redir);
		}
		if (t)
			t = t->next;
	}
	add_cmd(cmd_list, cmd);
	while (*tok && (*tok)->type != TOK_PIPE)
		*tok = (*tok)->next;
	if (*tok && (*tok)->type == TOK_PIPE)
		*tok = (*tok)->next;
	return (1);
}

/*
** Entry point of the parser.
** Iterates through all tokens and builds a linked list of t_cmd nodes
** for execution (one per command or pipe segment).
*/
int	parser(t_shell *shell)
{
	t_token	*curr;

	curr = g_tokens;
	while (curr)
	{
		if (!parse_command(&shell->cmds, &curr))
			return (0);
	}
	return (1);
}
