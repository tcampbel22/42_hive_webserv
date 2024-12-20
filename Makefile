NAME	:= webserv

CC		:= c++

FLAGS	:= -Wextra -Wall -Werror -std=c++17 -g #-fsanitize=address

OBJ_DIR := ./src/Objects
SRCS_DIR = ./src

SRCS	:= 	$(SRCS_DIR)/Server/main.cpp $(SRCS_DIR)/HttpParsing/HttpParser.cpp \
			$(SRCS_DIR)/HttpParsing/requestLineValidator.cpp $(SRCS_DIR)/HttpParsing/HttpHeaderParser.cpp \
			$(SRCS_DIR)/HttpParsing/chunkedBodyParser.cpp $(SRCS_DIR)/Server/HttpServer.cpp  \
			$(SRCS_DIR)/Response/Response.cpp $(SRCS_DIR)/Config/ConfigParser.cpp $(SRCS_DIR)/Config/ServerSettings.cpp \
			$(SRCS_DIR)/Config/LocationSettings.cpp $(SRCS_DIR)/Response/ServerHandler.cpp $(SRCS_DIR)/Config/ConfigUtilities.cpp \
			$(SRCS_DIR)/CGI/CGIparsing.cpp $(SRCS_DIR)/Config/ServerSettingsUtils.cpp $(SRCS_DIR)/Logger/Logger.cpp \
			$(SRCS_DIR)/Server/ServerUtils.cpp
		
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