/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 14:53:55 by nakhalil          #+#    #+#             */
/*   Updated: 2025/04/29 19:27:09 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// int main(int argc, char **argv, char **envp)
// {
//     t_data  data = {
//         .token_count = 0,
//         .cmd_count = 0,
//         .env = envp,
//         .last_status = 0
//     };
//     char    *input;

//     (void)argc;
//     (void)argv;
//     setup_signals();
//     while (1)
//     {
//         input = readline("minishell> ");
//         if (!input)
//         {
//             write(1, "exit\n", 5);
//             break;
//         }
//         if (*input)
//         {
//             add_history(input);
//             if (tokenize_input(input, &data) == 0 && validate_syntax(&data))
//             {
//                 expand_tokens(&data);
//                 parse_tokens(&data);
//             }
//             free(input);
//             free_data(&data);
//         }
//     }
//     return (0);
// }

/**
 * Print out all tokens for inspection.
//  */
// static void debug_print_tokens(t_data *data)
// {
//     printf("→ [debug] token_count = %d\n", data->token_count);
//     for (int i = 0; i < data->token_count; i++)
//     {
//         t_token *tok = &data->tokens[i];
//         printf("   token[%d]: type=%d, quote=%d, value=\"%s\"\n",
//                i, tok->type, tok->quote, tok->value);
//     }
// }

/**
 * Print out all parsed commands, their args and redirections.
 */
// static void debug_print_commands(t_data *data)
// {
//     printf("→ [debug] cmd_count   = %d\n", data->cmd_count);
//     for (int c = 0; c < data->cmd_count; c++)
//     {
//         t_command *cmd = &data->commands[c];
//         printf("   command[%d]:\n", c);

//         if (cmd->args)
//         {
//             for (int j = 0; cmd->args[j]; j++)
//                 printf("      arg[%d] = \"%s\"\n", j, cmd->args[j]);
//         }
//         else
//             printf("      (no args)\n");

//         for (int r = 0; r < cmd->redir_count; r++)
//         {
//             printf("      redir[%d]: type=%d, file=\"%s\"\n",
//                    r, cmd->redirs[r].type, cmd->redirs[r].file);
//         }
//     }
// }

int main(int argc, char **argv, char **envp)
{
    t_data  data = {
        .token_count = 0,
        .cmd_count   = 0,
        .env         = envp,
        .last_status = 0
    };
    char    *input;

    (void)argc;
    (void)argv;
    setup_signals();

    while (1)
    {
        input = readline("minishell> ");
        if (!input)
        {
            write(1, "exit\n", 5);
            break;
        }

        if (*input)
        {
            add_history(input);
            if (tokenize_input(input, &data) == 0
                && validate_syntax(&data))
            {
                // Debug dump: raw tokens
                // debug_print_tokens(&data);

                // Expand $vars, then parse into commands
                expand_tokens(&data);
                parse_tokens(&data);

                // Debug dump: parsed commands/redirects
                // debug_print_commands(&data);
            }
            free(input);
            free_data(&data);
        }
    }
    return (0);
}
