NAME		= ircserv

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

INC_DIR		= includes
SRC_DIR		= src
OBJ_DIR		= obj

SRCS_SRV		= Server.cpp
SRCS_SRV_PATH	= $(SRC_DIR)/server/
SRCS_CMD		= commands.cpp
SRCS_CMD_PATH	= $(SRC_DIR)/commands/
SRCS_CL			= Client.cpp
SRCS_CL_PATH	= $(SRC_DIR)/client/
SRCS_CH			= channel.cpp
SRCS_CH_PATH	= $(SRC_DIR)/channel/

SRCS		= 	$(addprefix $(SRCS_SRV_PATH), $(SRCS_SRV)) \
				$(addprefix $(SRCS_CMD_PATH), $(SRCS_CMD)) \
				$(addprefix $(SRCS_CL_PATH), $(SRCS_CL)) \
				$(addprefix $(SRCS_CH_PATH), $(SRCS_CH)) \
			 	 $(SRC_DIR)/main.cpp

OBJS		= $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)
	$(print_flag)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)🚀 $(NAME) BUILT SUCCESSFULLY! 🚀$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@
	@echo "$(YELLOW)COMPILING: $<$(RESET)"

clean:
	rm -rf $(OBJ_DIR)
	@echo "$(RED)🧹 CLEANED OBJECT FILES 🧹$(RESET)"

fclean: clean
	rm -f $(NAME)
	@echo "$(RED) 🧹🧹 FULL CLEAN COMPLETE 🧹🧹$(RESET)"

re: fclean all

.PHONY: all clean fclean re

# Colors
RESET	= \033[0m
BOLD	= \033[1m
GREEN	= \033[1;32m
BLUE	= \033[1;34m
RED		= \033[1;31m
YELLOW	= \033[1;33m

define print_flag
	@echo "$(GREEN) 🚀 COMPILATION COMPLETE 🚀$(RESET)"
	@echo "$(GREEN).☆.｡.:*・°･.｡*･.｡* .☆.｡.:*.☆.｡.:*・°･.｡*･.｡*.☆.｡.:*.☆.｡.:*.☆.｡.:*.☆.｡.:*.☆.｡.:*.☆.｡.:*.☆.｡.:*$(RESET)"
	@echo "$(BLUE)"
	@cat stuff/ascii_art.txt
	@echo ""
	@echo "$(RESET)"
	@echo "$(GREEN).☆.｡.:*・°･.｡*･.｡* .☆.｡.:*.☆.｡.:*・°･.｡*･.｡*.☆.｡.:*.☆.｡.:*.☆.｡.:*.☆.｡.:*.☆.｡.:*.☆.｡.:*.☆.｡.:*$(RESET)"
endef
