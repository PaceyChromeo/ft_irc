CC=clang++
FLAGS= -Wall -Werror -Wextra -g -std=c++98
#FLAGS += -g -fsanitize=address
NAME=ircserv
ifneq (,$(findstring xterm,${TERM}))
	BLACK        := $(shell tput -Txterm setaf 0)
	RED          := $(shell tput -Txterm setaf 1)
	GREEN        := $(shell tput -Txterm setaf 2)
	YELLOW       := $(shell tput -Txterm setaf 3)
	LIGHTPURPLE  := $(shell tput -Txterm setaf 4)
	PURPLE       := $(shell tput -Txterm setaf 5)
	BLUE         := $(shell tput -Txterm setaf 6)
	WHITE        := $(shell tput -Txterm setaf 7)
	RESET := $(shell tput -Txterm sgr0)
endif

SRC=	srcs/Channel.cpp \
		srcs/Command.cpp \
		srcs/main.cpp \
		srcs/Server.cpp \
		srcs/User.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(FLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	@printf "\033[0;33m Compiling : ${BLUE}$@ \r"
	@$(CC) $(FLAGS) -c $< -o $@ 
	

clean:
	@rm -f $(OBJ)

fclean: clean
	@echo "\033[0;33m Cleaning : \033[0;32m [DONE]"
	@rm -f $(NAME)

re: fclean all