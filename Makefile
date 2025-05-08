# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/23 15:41:33 by nakhalil          #+#    #+#              #
#    Updated: 2025/05/08 14:03:23 by nakhalil         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

# Compiler
CC = cc
CFLAGS = -Wall -Wextra -Werror

# Source files
SRCS = main.c tokenizer.c parser.c signals.c utils.c expander.c syntax_check.c syntax_utils.c

# Object files
OBJS = $(SRCS:.c=.o)

# Includes
INCLUDES = -I. -I./libft

# Libraries
LIBS = -L./libft -lft -lreadline -I.brew/opt/readline/include -L/opt/vagrant/embedded/lib -lreadline

# Libft
LIBFT_DIR = ./libft
LIBFT = $(LIBFT_DIR)/libft.a

# Rules
all: $(LIBFT) $(NAME)

$(LIBFT):
	@make -C $(LIBFT_DIR)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(NAME) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)
	make clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME)
	make fclean -C $(LIBFT_DIR)

re: fclean all

.PHONY: all clean fclean re
