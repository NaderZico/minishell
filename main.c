#include "minishell.h"

/* ───────────────────────────────────────────────────────────────
 * Print all parsed tokens (for debugging)
 * ─────────────────────────────────────────────────────────────── */
void	print_tokens(t_token *t)
{
	while (t)
	{
		printf("TOKEN: [%s] type=%d\n", t->value, t->type);
		t = t->next;
	}
}

/* ───────────────────────────────────────────────────────────────
 * Print parsed commands (for parser testing)
 * ─────────────────────────────────────────────────────────────── */
void	print_cmds(t_cmd *cmd)
{
	int	i;

	while (cmd)
	{
		printf("CMD:\n");
		i = 0;
		while (cmd->argv && cmd->argv[i])
		{
			printf("  argv[%d] = [%s]\n", i, cmd->argv[i]);
			i++;
		}
		t_redir *r = cmd->redir;
		while (r)
		{
			printf("  redir: type=%d -> [%s]\n", r->type, r->filename);
			r = r->next;
		}
		cmd = cmd->next;
	}
}

/* ───────────────────────────────────────────────────────────────
 * Free everything after each loop
 * ─────────────────────────────────────────────────────────────── */
void	free_redirs(t_redir *r)
{
	t_redir *tmp;

	while (r)
	{
		tmp = r->next;
		free(r->filename);
		free(r);
		r = tmp;
	}
}

void	free_cmds(t_cmd *cmd)
{
	t_cmd *tmp;
	int		i;

	while (cmd)
	{
		tmp = cmd->next;
		if (cmd->argv)
		{
			i = 0;
			while (cmd->argv[i])
				free(cmd->argv[i++]);
			free(cmd->argv);
		}
		free_redirs(cmd->redir);
		free(cmd);
		cmd = tmp;
	}
}

void	free_token_list(t_token *tokens)
{
	t_token *tmp;

	while (tokens)
	{
		tmp = tokens->next;
		free(tokens->value);
		free(tokens);
		tokens = tmp;
	}
}

void	free_all(t_shell *shell)
{
	free(shell->line);
	free_cmds(shell->cmds);
	free_token_list(g_tokens);
	shell->line = NULL;
	shell->cmds = NULL;
	g_tokens = NULL;
}

/* ───────────────────────────────────────────────────────────────
 * REPL loop for testing lexer and parser
 * ─────────────────────────────────────────────────────────────── */
int	main(void)
{
	t_shell	shell;

	shell.line = NULL;
	shell.cmds = NULL;

	while (1)
	{
		shell.line = readline("minishell$ ");
		if (!shell.line)
		{
			printf("exit\n");
			break;
		}
		if (*shell.line)
			add_history(shell.line);
		if (lexer(&shell))
		{
			if (parser(&shell))
				print_cmds(shell.cmds);
		}
		free_all(&shell);
	}
	return (0);
}
