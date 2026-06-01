CPP = c++

CPP_FLAGS =  -std=c++98 -Wall -Wextra -Werror -g

SRCS := $(wildcard ./srcs/parser/*.cpp) \
				$(wildcard ./srcs/Server/*.cpp) \
			 ./srcs/main.cpp

INCLUDE_DIR = ./include/

OBJS = $(SRCS:%.cpp=%.o)

NAME = parser
TARGET = $(NAME)

$(TARGET): $(NAME)

$(NAME): $(OBJS)
	echo $(CRCS)
	$(CPP) $(CPP_FLAGS) -o $@ $^

%.o: %.cpp
	$(CPP) $(CPP_FLAGS) -I $(INCLUDE_DIR)  -o $@ -c $<

clean:
	$(RM) $(RM_OPTIONS) $(OBJS)

fclean: clean
	$(RM) $(RM_OPTIONS) $(NAME)

re: fclean $(TARGET)

test: $(TARGET)
	./$(TARGET) ./config/test_file.conf
