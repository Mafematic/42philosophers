# Compiler and flags
CC = cc
CFLAGS = -Wall -Werror -Wextra

# Source dir and files
SRC_DIR = src
SRCS = src/check_arguments.c src/main.c src/output.c src/philosophers.c src/time.c src/thread_routines.c

# Object files
OBJ_DIR = obj
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Output binary
TARGET = philo

# Phony targets
.PHONY: all clean fclean re

# Default target (all)
all: $(TARGET)

# Linking the program
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Compilation rule for object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean object files
clean:
	rm -rf $(OBJ_DIR)

# Remove all generated files
fclean: clean
	rm -f $(TARGET)

# Rebuild everything
re: fclean all