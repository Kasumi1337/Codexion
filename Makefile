NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread
RM			= rm -f

SRCS		= main.c \
			  parsing.c \
			  parsing_utils.c \
			  utils.c \
			  init.c \
			  dongle.c \
			  dongle_utils.c \
			  heap.c \
			  heap_ops.c \
			  sync.c \
			  coder.c \
			  monitor.c \
			  cleanup.c

OBJS		= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
