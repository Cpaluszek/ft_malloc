ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME := libft_malloc_$(HOSTTYPE).so
SYMLINK := libft_malloc.so

SRC_DIR := src
INCLUDE_DIR := inc
BUILD_DIR := build

SRC_FILES := malloc.c
SRCS := $(addprefix $(SRC_DIR)/,$(SRC_FILES))
OBJS := $(SRC_FILES:%.c=$(BUILD_DIR)/%.o)
DEPS := $(SRC_FILES:%.c=$(BUILD_DIR)/%.d)
CC_DEFS := NAME=\"$(NAME)\"

CC := gcc
CC_FLAGS := -Wextra -Wall -Werror -fPIC -g3
LD_FLAGS := -shared

CC_DEPS_FLAGS	:=	-MP -MMD
CC_DEFS_FLAGS	:=	$(foreach def,$(CC_DEFS),-D $(def))

LIBFT_DIR := libft
LIBFT := $(LIBFT_DIR)/libft.a

all: $(NAME) $(SYMLINK)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(LD_FLAGS) $(OBJS) $(CXX_LIBS) $(LIBFT) -o $@

$(SYMLINK): $(NAME)
	@ln -sf $(NAME) $(SYMLINK)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS) $(CC_DEPS_FLAGS) $(CC_DEFS_FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(LIBFT): force
	$(MAKE) -C $(LIBFT_DIR)

-include $(DEPS)

clean:
	@rm -rf $(BUILD_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@rm -f $(NAME) $(SYMLINK)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

test: force all
	$(MAKE) -C test
	LD_PRELOAD=./libft_malloc.so ./test/test

force:

.PHONY: all clean fclean re
