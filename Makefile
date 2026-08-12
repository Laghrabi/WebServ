CPP = c++

CPP_FLAGS =  -std=c++98 -Wall -Wextra -Werror -g #-fsanitize=address

SRCS := ./srcs/HTTP/request/HttpRequestValidation.cpp \
./srcs/HTTP/request/HttpRequestGetters.cpp \
./srcs/HTTP/request/HttpRequest.cpp \
./srcs/HTTP/request/HttpRequestHelpers.cpp \
./srcs/HTTP/response/HttpRequestHandler.cpp \
./srcs/HTTP/response/HttpResponse.cpp \
./srcs/HTTP/router/ResourceLocator.cpp \
./srcs/HTTP/router/RouteManager.cpp \
./srcs/Utils/StringUtils.cpp \
./srcs/Utils/join_path.cpp \
./srcs/cgi/execution/CgiHandler.cpp \
./srcs/cgi/request/CgiRequest.cpp \
./srcs/multyplexing/ListeningSockeet.cpp \
./srcs/multyplexing/ClientSocket.cpp \
./srcs/multyplexing/ConnectionManager.cpp \
./srcs/parser/Config.cpp \
./srcs/parser/Location.cpp \
./srcs/parser/tokenization.cpp \
./srcs/parser/RouteNode.cpp \
./srcs/parser/IPort.cpp \
./srcs/parser/IPortV4.cpp \
./srcs/parser/ParseConfig.cpp \
./srcs/parser/RouteConfig.cpp \
./srcs/parser/IPortV6.cpp \
./srcs/parser/MimeTypesExt.cpp \
./srcs/parser/Server.cpp \
./srcs/util/FileStatus.cpp \
./srcs/util/copyArraytoVec.cpp \
./srcs/util/printConfig.cpp \
./srcs/util/safeClose.cpp \
./srcs/util/compare_headers.cpp \
./srcs/util/to_string.cpp \
./srcs/CookiesAndSession/Cookies.cpp \
./srcs/CookiesAndSession/Session.cpp \
./srcs/CookiesAndSession/SessionManager.cpp \
./srcs/main.cpp 

INCLUDE_DIR = ./include/

OBJS = $(SRCS:%.cpp=%.o)

NAME = webserver

$(NAME): $(OBJS)
	$(CPP) $(CPP_FLAGS) -o $@ $^

DEBUG: CPP_FLAGS += -D CGI_DEBUG="true" -D DEBUG
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

# .PHONY: $(NAME)


