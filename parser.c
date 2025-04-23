/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 19:50:12 by nakhalil          #+#    #+#             */
/*   Updated: 2025/04/22 19:50:14 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern t_token *g_tokens;

// ─────────────────────────────────────────────────────────────────────────────
// Helpers to build redirection and command structures
// ─────────────────────────────────────────────────────────────────────────────

static t_redir	*new_redir(int type, char *filename)
{
	t_redir *r = malloc(sizeof(t_redir));
	if (!r)
		return (NULL);
	r->type = type;
	r->filename = strdup(filename);
	r->next = NULL;
	return (r);
}

static void	add_redir(t_redir **redir_list, t_redir *new)
{
	if (!*redir_list)
		*redir_list = new;
	else
	{
		t_redir *tmp = *redir_list;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

static t_cmd	*new_cmd(void)
{
	t_cmd *cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->redir = NULL;
	cmd->next = NULL;
	cmd->infile = STDIN_FILENO;
	cmd->outfile = STDOUT_FILENO;
	cmd->pipe_fd[0] = -1;
	cmd->pipe_fd[1] = -1;
	cmd->pid = -1;
	return (cmd);
}

static void	add_cmd(t_cmd **cmd_list, t_cmd *new)
{
	if (!*cmd_list)
		*cmd_list = new;
	else
	{
		t_cmd *cur = *cmd_list;
		while (cur->next)
			cur = cur->next;
		cur->next = new;
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// Argument collection & counting
// ─────────────────────────────────────────────────────────────────────────────

static int	count_args(t_token *tok)
{
	int count = 0;

	while (tok && tok->type != TOK_PIPE)
	{
		if (tok->type == TOK_WORD)
			count++;
		else if (tok->type >= TOK_REDIR_IN && tok->type <= TOK_REDIR_HEREDOC)
			tok = tok->next; // skip redir target
		tok = tok ? tok->next : NULL;
	}
	return count;
}

static char	**collect_args(t_token **tok_ptr)
{
	int i = 0;
	int argc = count_args(*tok_ptr);
	char **argv = calloc(argc + 1, sizeof(char *));
	if (!argv)
		return (NULL);

	while (*tok_ptr && (*tok_ptr)->type != TOK_PIPE)
	{
		if ((*tok_ptr)->type == TOK_WORD)
		{
			argv[i++] = strdup((*tok_ptr)->value);
		}
		else if ((*tok_ptr)->type >= TOK_REDIR_IN && (*tok_ptr)->type <= TOK_REDIR_HEREDOC)
		{
			*tok_ptr = (*tok_ptr)->next;
			if (!*tok_ptr || (*tok_ptr)->type != TOK_WORD)
			{
				fprintf(stderr, "Syntax error near redirection\n");
				return (NULL);
			}
		}
		*tok_ptr = (*tok_ptr)->next;
	}
	argv[i] = NULL;
	return argv;
}

// ─────────────────────────────────────────────────────────────────────────────
// Parse one command (until pipe or end)
// ─────────────────────────────────────────────────────────────────────────────

static int	parse_command(t_cmd **cmd_list, t_token **tok)
{
	t_cmd *cmd = new_cmd();
	if (!cmd)
		return (0);

	t_token *t = *tok;
	cmd->argv = collect_args(&t);
	if (!cmd->argv)
		return (0);

	t = *tok; // reset to parse redirs again
	while (t && t->type != TOK_PIPE)
	{
		if (t->type >= TOK_REDIR_IN && t->type <= TOK_REDIR_HEREDOC)
		{
			int redir_type = t->type;
			t = t->next;
			if (!t || t->type != TOK_WORD)
			{
				fprintf(stderr, "Missing filename after redirection\n");
				return (0);
			}
			add_redir(&cmd->redir, new_redir(redir_type, t->value));
		}
		t = t ? t->next : NULL;
	}

	if (*tok && (*tok)->type == TOK_PIPE)
		*tok = (*tok)->next;
	else
		*tok = NULL;

	add_cmd(cmd_list, cmd);
	return (1);
}

// ─────────────────────────────────────────────────────────────────────────────
// Main parser entry point
// ─────────────────────────────────────────────────────────────────────────────

int	parser(t_shell *shell)
{
	t_token *cur = g_tokens;

	while (cur)
	{
		if (!parse_command(&shell->cmds, &cur))
			return (0);
	}
	return (1);
}
