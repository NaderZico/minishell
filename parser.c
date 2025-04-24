#include "minishell.h"

extern t_token *g_tokens;

/* ───────────────────────────────────────────────────────────────
 * Helpers: t_cmd and t_redir constructors
 * ─────────────────────────────────────────────────────────────── */
static t_cmd	*new_cmd(void)
{
	t_cmd	*cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->redir = NULL;
	cmd->infile = STDIN_FILENO;
	cmd->outfile = STDOUT_FILENO;
	cmd->pipe_fd[0] = -1;
	cmd->pipe_fd[1] = -1;
	cmd->pid = -1;
	cmd->next = NULL;
	return (cmd);
}

static void	add_cmd(t_cmd **cmd_list, t_cmd *new)
{
	if (!*cmd_list)
		*cmd_list = new;
	else
	{
		t_cmd *tmp = *cmd_list;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

static t_redir	*new_redir(int type, char *filename)
{
	t_redir *redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->filename = strdup(filename);
	redir->next = NULL;
	return (redir);
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

/* ───────────────────────────────────────────────────────────────
 * Count args and build argv[]
 * ─────────────────────────────────────────────────────────────── */
static int	count_args(t_token *tok)
{
	int	count = 0;

	while (tok && tok->type != TOK_PIPE)
	{
		if (tok->type == TOK_WORD)
			count++;
		else if (tok->type >= TOK_REDIR_IN && tok->type <= TOK_REDIR_HEREDOC)
			tok = tok->next;
		tok = tok ? tok->next : NULL;
	}
	return (count);
}

static char	**build_argv(t_token *tok)
{
	int		i = 0;
	int		argc = count_args(tok);
	char	**argv = calloc(argc + 1, sizeof(char *));
	if (!argv)
		return (NULL);

	while (tok && tok->type != TOK_PIPE)
	{
		if (tok->type == TOK_WORD)
			argv[i++] = strdup(tok->value);
		else if (tok->type >= TOK_REDIR_IN && tok->type <= TOK_REDIR_HEREDOC)
			tok = tok->next;
		tok = tok ? tok->next : NULL;
	}
	argv[i] = NULL;
	return (argv);
}

/* ───────────────────────────────────────────────────────────────
 * Parse one command: builds argv and redir
 * Advances token pointer up to (and including) pipe if needed
 * ─────────────────────────────────────────────────────────────── */
static int	parse_command(t_cmd **cmd_list, t_token **tok)
{
	t_cmd	*cmd;
	t_token	*t;

	if (!tok || !*tok)
		return (0);
	cmd = new_cmd();
	if (!cmd)
		return (0);

	cmd->argv = build_argv(*tok);
	if (!cmd->argv)
		return (0);

	// Parse redirections
	t = *tok;
	while (t && t->type != TOK_PIPE)
	{
		if (t->type >= TOK_REDIR_IN && t->type <= TOK_REDIR_HEREDOC)
		{
			int	type = t->type;
			t = t->next;
			if (!t || t->type != TOK_WORD)
			{
				fprintf(stderr, "Syntax error near redirection\n");
				return (0);
			}
			add_redir(&cmd->redir, new_redir(type, t->value));
		}
		t = t ? t->next : NULL;
	}

	add_cmd(cmd_list, cmd);

	// Advance *tok past the pipe (if any)
	while (*tok && (*tok)->type != TOK_PIPE)
		*tok = (*tok)->next;
	if (*tok && (*tok)->type == TOK_PIPE)
		*tok = (*tok)->next;

	return (1);
}

/* ───────────────────────────────────────────────────────────────
 * Main parser entry: builds full t_cmd list in shell->cmds
 * ─────────────────────────────────────────────────────────────── */
int	parser(t_shell *shell)
{
	t_token	*cur = g_tokens;

	while (cur)
	{
		if (!parse_command(&shell->cmds, &cur))
			return (0);
	}
	return (1);
}
