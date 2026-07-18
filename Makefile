CPP = c++

CPP_FLAGS =  -std=c++98 -Wall -Wextra -Werror -g -fPIE

SRCS := $(wildcard ./srcs/parser/*.cpp) \
				$(wildcard ./srcs/util/*.cpp) \
			 ./srcs/main.cpp \
			 $(wildcard ./srcs/HTTP/request/*.cpp)\
				$(wildcard ./srcs/multyplexing/*.cpp)\
				$(wildcard ./srcs/Utils/*.cpp)\
				$(wildcard ./srcs/HTTP/router/*.cpp)\

INCLUDE_DIR = ./include/

OBJS = $(SRCS:%.cpp=%.o)

NAME = webserver

$(NAME): $(OBJS)
	$(CPP) $(CPP_FLAGS) -o $@ $^

DEBUG: CPP_FLAGS += -D DEBUG="true"
DEBUG: $(NAME)

%.o: %.cpp
	$(CPP) $(CPP_FLAGS) -I $(INCLUDE_DIR)  -o $@ -c $<

clean:
	$(RM) $(RM_OPTIONS) $(OBJS)

fclean: clean
	$(RM) $(RM_OPTIONS) $(NAME)

re: fclean $(NAME)

test: $(NAME)
	./$(NAME) ./test_config/test.conf

.PHONY: $(NAME)
