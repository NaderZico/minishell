/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 19:32:58 by nakhalil          #+#    #+#             */
/*   Updated: 2025/05/17 19:50:41 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t g_signal = 0;

/**
 * sigint_handler
 *   Called on Ctrl-C in the interactive shell. Prints a newline,
 *   resets the readline prompt, and updates the shell's last status.
 */
static void	sigint_handler(int sig)
{
    (void)sig;
    write(1, "\n", 1);
    rl_replace_line("", 0);
    rl_on_new_line();
    rl_redisplay();
    g_signal = 130;  // POSIX standard for SIGINT
}

/**
 * setup_signals
 *   Installs sigint_handler for SIGINT, ignores SIGQUIT.
 */
void	setup_signals(void)
{
    struct sigaction	sa;
    struct sigaction	sa_quit;

    sa.sa_handler = sigint_handler;
    sa.sa_flags = SA_RESTART | SA_NODEFER;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) < 0)
    {
        ft_putstr_fd("minishell: failed to set SIGINT handler\n", 2);
        exit(EXIT_FAILURE);
    }

    sa_quit.sa_handler = SIG_IGN;
    sa_quit.sa_flags = 0;
    sigemptyset(&sa_quit.sa_mask);
    if (sigaction(SIGQUIT, &sa_quit, NULL) < 0)
    {
        ft_putstr_fd("minishell: failed to set SIGQUIT handler\n", 2);
        exit(EXIT_FAILURE);
    }
}
