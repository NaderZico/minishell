/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 19:32:58 by nakhalil          #+#    #+#             */
/*   Updated: 2025/04/25 20:30:25 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define RL_USE_VISIBLE_PROMPT 1
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "minishell.h"

void rl_replace_line(char *line, int clear_undo);

static void	sigint_handler(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
    rl_replace_line("", 0);
	rl_redisplay();
}

static void	sigquit_handler(int sig)
{
	(void)sig;
	/* do nothing (ignored in parent) */
}

void	init_signals(void)
{
	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, sigquit_handler);
}

void	set_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}