NAME		:=	webserv
FLAGS		=	-Wall -Wextra -Werror -std=c++98 -pedantic

ifdef DEBUG
FLAGS+= -g -fsanitize=address
endif

LOG_FILE=

ifdef LOG
LOG_FILE+=  2>&1 | tee webserv.log
endif

INCLUDES	:=	-Iincludes

SRC_DIR		:= 	srcs
OBJ_DIR		:=	objs

OBJ =	main
OBJ +=	Authentication
OBJ +=	base64
OBJ +=	Cgi
OBJ +=	Client
OBJ +=	RequestHandler
OBJ +=	RequestHandlerMimeTypes
OBJ +=	RequestHandlerStatusCodes
OBJ +=	Server
OBJ +=	ServerAccept
OBJ +=	ServerInit
OBJ +=	ServerReceive
OBJ +=	ServerRespond
OBJ +=	ServerRun
OBJ +=	utils
OBJ +=	VirtualServer

OBJ			:= $(addsuffix .o, $(addprefix $(OBJ_DIR)/, $(OBJ)))

CC 			:= clang++

define NL


endef

all: $(NAME)

$(NAME): $(OBJ)
	@$(foreach obj, $?, echo Linking $(notdir $(obj))$(NL))
	@$(CC) $(FLAGS) $(OBJ) -g -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER)
	@mkdir -p $(OBJ_DIR)/$(dir $*)
	@echo Compiling $(notdir $<)
	@$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	@$(RM) -r $(wildcard $(OBJ_DIR))

fclean:
	@$(MAKE) clean
	$(RM) $(wildcard $(NAME))

re:
	$(MAKE) fclean
	$(MAKE) all

run: $(NAME)
		./$(NAME) $(LOG_FILE)

.PHONY: all clean fclean re
