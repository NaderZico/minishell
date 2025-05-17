# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/23 15:41:33 by nakhalil          #+#    #+#              #
#    Updated: 2025/05/17 15:32:56 by nakhalil         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

# Compiler
CC = cc
CFLAGS = -Wall -Wextra -Werror

# Directories
SRCDIR = .
BUILDDIR = build
LIBFT_DIR = ./libft

# Source files (explicitly listed)
SRCS = main.c \
	tokenizer.c \
	parser.c \
	signals.c \
	utils.c \
	utils_free.c \
	expander.c \
	syntax_check.c \
	syntax_utils.c

# Object files in build/
OBJS = $(SRCS:%.c=$(BUILDDIR)/%.o)

# Includes and libraries
INCLUDES = -I. -I./libft
LIBS = -L./libft -lft -lreadline -I.brew/opt/readline/include -L/opt/vagrant/embedded/lib -lreadline
LIBFT = $(LIBFT_DIR)/libft.a

# Rules
all: $(LIBFT) $(NAME)

$(LIBFT):
	@make -C $(LIBFT_DIR)

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(NAME) $(OBJS) $(LIBS)

clean:
	rm -rf $(BUILDDIR)
	make clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME)
	make fclean -C $(LIBFT_DIR)

re: fclean all

.PHONY: all clean fclean re
