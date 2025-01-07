NAME := test

CC := gcc
CFLAGS := -Wextra -Wall -Werror -g3

SRC_FILES := test.c

all: $(NAME)

$(NAME): $(SRC_FILES)
	$(CC) $(CFLAGS) -o $(NAME) $(SRC_FILES) -L.. -lft_malloc -lcriterion -I../inc

clean:
	@rm -f $(NAME)

re: clean all

.PHONY: all clean re
