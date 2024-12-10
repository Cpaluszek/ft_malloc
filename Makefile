ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME := libft_malloc_$(HOSTTYPE).so
SYMLINK := libft_malloc.so

SRC_DIR := src
INCLUDE_DIR := inc
BUILD_DIR := build

SRC_FILES := malloc.c free.c
SRCS := $(addprefix $(SRC_DIR)/,$(SRC_FILES))
OBJS := $(SRC_FILES:%.c=$(BUILD_DIR)/%.o)
DEPS := $(SRC_FILES:%.c=$(BUILD_DIR)/%.d)

CC := gcc
CC_FLAGS := -Wextra -Wall -Werror -fPIC
LD_FLAGS := -shared

all: $(NAME) $(SYMLINK)

$(NAME): $(OBJS)
	$(CC) $(LD_FLAGS) $(OBJS) $(CXX_LIBS) -o $@

$(SYMLINK): $(NAME)
	@ln -sf $(NAME) $(SYMLINK)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

-include $(DEPS)

clean:
	@rm -rf $(BUILD_DIR)

fclean: clean
	@rm -f $(NAME) $(SYMLINK)

re: fclean all

.PHONY: all clean fclean re
