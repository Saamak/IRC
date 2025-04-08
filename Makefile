NAME = ircserv
OBJ_DIR = obj

SRC = main.cpp \
	server.cpp \
	commands.cpp \
	channel.cpp \
	config.cpp \
	client.cpp \
	Mode.cpp \
	Join.cpp \
	User.cpp \
	Nick.cpp \
	Pass.cpp \
	PrivMsg.cpp \
	Invite.cpp \
	Who.cpp \
	Kick.cpp \
	Topic.cpp \
	ModeSignMinus.cpp \
	ModeSignPlus.cpp \
	Part.cpp

# Préfixer tous les objets avec le dossier obj/
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

all: $(OBJ_DIR) $(NAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re $(OBJ_DIR)