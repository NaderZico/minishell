#include "minishell.h"

// ───────────────────────────────────────────────────────────────
// Temporary debug function to print tokens
// ───────────────────────────────────────────────────────────────
void	print_tokens(t_token *t)
{
	while (t)
	{
		printf("TOKEN: [%s] type=%d\n", t->value, t->type);
		t = t->next;
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

// ───────────────────────────────────────────────────────────────
// Free input line and tokens between loops
// ───────────────────────────────────────────────────────────────
void	free_all(t_shell *shell)
{
	if (shell->line)
	{
		free(shell->line);
		shell->line = NULL;
	}
	if (g_tokens)
	{
		free_token_list(g_tokens);
		g_tokens = NULL;
	}
}

// ───────────────────────────────────────────────────────────────
// Main REPL loop
// ───────────────────────────────────────────────────────────────
int	main(void)
{
	t_shell	shell;

	shell.line = NULL;

	while (1)
	{
		shell.line = readline("minishell$ ");
		if (!shell.line) // Ctrl-D or EOF
		{
			printf("exit\n");
			break;
		}
		if (*shell.line)
			add_history(shell.line);
		if (lexer(&shell))
			print_tokens(g_tokens);
		free_all(&shell);
	}
	return (0);
}
