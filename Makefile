NAME = ircserv

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
	  Kick.cpp \
	  Topic.cpp \
	  ModeSignMinus.cpp \
	  ModeSignPlus.cpp

OBJ = $(SRC:.cpp=.o)

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re