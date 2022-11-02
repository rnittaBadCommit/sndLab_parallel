NAME_SERVER="server"
NAME_CLIENT="client"

SRCS_SERVER=srcs/server.cpp srcs/server_main.cpp srcs/request.cpp srcs/utils.cpp
SRCS_CLIENT=srcs/client.cpp

OBJS_SERVER=$(SRCS_SERVER:.cpp=.o)
DEPS_SERVER=$(SRCS_SERVER:.cpp=.d)

OBJS_CLIENT=$(SRCS_CLIENT:.cpp=.o)
DEPS_CLIENT=$(SRCS_CLIENT:.cpp=.d)

CXX=c++
CXXFLAG=-std=c++11 -MMD

.cpp.o:
	$(CXX) $(CXXFLAG) -c $< -o $@

all: $(NAME_SERVER) $(NAME_CLIENT)

$(NAME_SERVER): $(OBJS_SERVER)
	$(CXX) $(CXXFLAG) $(OBJS_SERVER) -o $(NAME_SERVER)

$(NAME_CLIENT): $(OBJS_CLIENT)
	$(CXX) $(CXXFLAG) $(OBJS_CLIENT) -o $(NAME_CLIENT)

clean:
	rm -rf $(OBJS_CLIENT)
	rm -rf $(OBJS_SERVER)

fclean: clean
	rm -rf NAME_SERVER
	rm -rf NAME_CLIENT

-include $(DEPS_SERVER) $(DEPS_CLIENT)

.PHONY: all clean fclean re server client $(NAME_SERVER) $(NAME_CLIENT)
