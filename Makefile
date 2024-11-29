NAME	:= webserv

CC		:= c++

FLAGS	:= -Wextra -Wall -Werror -std=c++20 #-fsanitize=address -g

OBJ_DIR := ./src/Objects
SRCS_DIR = ./src

SRCS	:= $(SRCS_DIR)/Server/main.cpp $(SRCS_DIR)/HttpParsing/HttpParser.cpp \
			$(SRCS_DIR)/HttpParsing/src/HttpParsing/requestLineValidator.cpp $(SRCS_DIR)/HttpParsing/src/HttpParsing/src/HttpParsing/HttpHeaderParser.cpp \
			$(SRCS_DIR)/HttpParsing/src/HttpParsing/chunkedBodyParser.cpp $(SRCS_DIR)/Server/HttpServer.cpp  \
			$(SRCS_DIR)/Response/Response.cpp $(SRCS_DIR)/Config/ConfigParser.cpp $(SRCS_DIR)/Config/ServerSettings.cpp \
			$(SRCS_DIR)/Config/LocationSettings.cpp $(SRCS_DIR)/Response/ServerHandler.cpp $(SRCS_DIR)/Config/ConfigUtilities.cpp
		
OBJS	:= $(addprefix $(OBJ_DIR)/, $(notdir ${SRCS:.cpp=.o}))

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/*/%.cpp | $(OBJ_DIR)
	@$(CC) -o $@ -c $< $(FLAGS) && printf "Compiling: $(notdir $<)\n"
			
$(NAME): $(OBJS)
	@$(CC) $(FLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
