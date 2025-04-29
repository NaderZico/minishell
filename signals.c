/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 19:32:58 by nakhalil          #+#    #+#             */
/*   Updated: 2025/04/29 19:34:46 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t g_signal = 0;

int		rl_replace_line(const char *line, int clear_undo);

/**
 * sigint_handler
 *   Called on Ctrl-C in the interactive shell.  Prints a newline,
 *   resets the readline prompt, and sets an internal global to 130.
 */
void sigint_handler(int sig)
{
    (void)sig;
    write(1, "\n", 1);
    rl_replace_line("", 0);
    rl_on_new_line();
    rl_redisplay();
    g_signal = 130;
}

/**
 * setup_signals
 *   Installs sigint_handler for SIGINT, ignores SIGQUIT.
 */
void setup_signals(void)
{
    struct sigaction sa;
    
    sa.sa_handler = sigint_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    signal(SIGQUIT, SIG_IGN);
}
