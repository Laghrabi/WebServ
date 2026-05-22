CPP = c++

CPP_FLAGS = -std=c++98 -Wall -Wextra -Werror

SRCS := $(wildcard ./srcs/parser/*.cpp) \
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

flcean: clean
	$(RM) $(RM_OPTIONS) $(NAME)

re: fclean $(TARGET)

test: $(TARGET)
	./$(TARGET) ./config/test_file.conf
