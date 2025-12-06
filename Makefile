CFLAGS = -Wall -Wextra -Werror -std=c++98
OBJ_DIR = obj
SRC_DIR = src
INC_DIR = inc
COMP = c++
CONFIG = config.conf

SRC_FILES := $(shell find $(SRC_DIR) -type f | grep '\.cpp$$')

OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

RM  = rm -rf

NAME = webserver.exe

all : $(NAME)

$(NAME) : $(OBJ_FILES)
	@echo "🚀 Linking $(NAME)"
	@$(COMP) $(CFLAGS) $^ -o $@
	@chmod +x $(NAME)
	@echo "✅ $(NAME) built successfully!"

$(OBJ_DIR)/%.o :  $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "🔧 Compiling $<"
	@$(COMP) -I$(INC_DIR) $(CFLAGS) -c $< -o $@

clean :
	@echo "🧹 Cleaning up..."
	@$(RM) $(OBJ_DIR)
	@echo "✅ Cleaned up!"

fclean : clean
	@echo "🧼 Removing $(NAME)..."
	@$(RM) $(NAME)
	@echo "✅ fully cleaned!"

re: fclean all
	@echo "🔄 Rebuild completed!"

run : re
	@clear
	@echo "🎯 Running $(NAME)..."
	@$(RM) $(OBJ_DIR)
	@./$(NAME) $(CONFIG)

val : re
	@clear
	@echo "🔍 Running $(NAME) with Valgrind..."
	@$(RM) $(OBJ_DIR)
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) $(CONFIG)

.PHONY: all clean fclean re