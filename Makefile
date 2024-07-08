CC = gcc
CFLAGS = -Wall -Wextra -Werror -I include
LDFLAGS = -lm

SRC = src/ft_ping.c src/parse_options.c src/utils.c src/send_ping.c
OBJ = $(SRC:.c=.o)
NAME = ft_ping

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
