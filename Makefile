ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME := libft_malloc_$(HOSTTYPE).so
SYMLINK := libft_malloc.so

SRC_DIR := src
INCLUDE_DIR := inc
BUILD_DIR := build

SRC_FILES := malloc.c show_alloc_mem.c zone.c
SRCS := $(addprefix $(SRC_DIR)/,$(SRC_FILES))
OBJS := $(SRC_FILES:%.c=$(BUILD_DIR)/%.o)
DEPS := $(SRC_FILES:%.c=$(BUILD_DIR)/%.d)
CC_DEFS := NAME=\"$(NAME)\"

CC := gcc
CC_FLAGS := -Wextra -Wall -Werror -fPIC
LD_FLAGS := -shared

CC_DEPS_FLAGS	:=	-MP -MMD
CC_DEFS_FLAGS	:=	$(foreach def,$(CC_DEFS),-D $(def))

LINKAGE = $(SYMLINK)

DEBUG ?= 1

ifeq ($(DEBUG), 1)
CC_FLAGS += -g3 -fsanitize=address
LD_FLAGS += -fsanitize=address
LINKAGE = $(shell gcc -print-file-name=libasan.so):$(SYMLINK)
else
CC_FLAGS += -O2
LD_FLAGS +=
endif

all: $(NAME) $(SYMLINK)

$(NAME): $(OBJS)
	$(CC) $(LD_FLAGS) $(OBJS) $(CXX_LIBS) -o $@

$(SYMLINK): $(NAME)
	@ln -sf $(NAME) $(SYMLINK)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS) $(CC_DEPS_FLAGS) $(CC_DEFS_FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

-include $(DEPS)

clean:
	@rm -rf $(BUILD_DIR)
	$(MAKE) -C test clean

fclean: clean
	rm -f $(NAME) $(SYMLINK)

re: fclean all

test: all
	$(MAKE) -C test DEBUG=$(DEBUG)
	@echo "--- TESTS ---"
	LD_PRELOAD=$(LINKAGE) LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH ./test/test --verbose=0

.PHONY: all clean fclean re
