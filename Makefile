# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/23 15:41:33 by nakhalil          #+#    #+#              #
#    Updated: 2025/04/24 13:33:39 by nakhalil         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

# Compiler
CC = cc
CFLAGS = -Wall -Wextra -Werror

# Source files
SRCS = main.c lexer.c parser.c

# Object files
OBJS = $(SRCS:.c=.o)

# Includes
INCLUDES = -I. -I./libft

# Libraries
LIBS = -lreadline -L./libft -lft

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
