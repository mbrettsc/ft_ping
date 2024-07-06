NAME = ft_ping
SRC = src/ft_ping.c src/parse_options.c src/dns_lookup.c src/utils.c
CC = gcc
CFLAGS = -Wall -Wextra -Werror -I
INCLUDE = include

all: $(NAME)

$(NAME):
	$(CC) $(CFLAGS) $(INCLUDE) $(SRC) -o $(NAME)

clean:
	rm -rf $(NAME)


re: clean all

.PHONY: all clean re
