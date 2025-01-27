NAME := test

CC := gcc
CC_FLAGS := -Wextra -Wall -Werror -g3

DEBUG ?= 0

ifeq ($(DEBUG), 1)
CC_FLAGS += -fsanitize=address
endif

SRC_FILES := test.c

all: $(NAME)

$(NAME): $(SRC_FILES)
	$(CC) $(CC_FLAGS) -o $(NAME) $(SRC_FILES) -L.. -lft_malloc -lcriterion -I../inc

clean:
	@rm -f $(NAME)

re: clean all

.PHONY: all clean re
