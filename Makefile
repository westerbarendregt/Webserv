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
OBJ +=	utils
OBJ +=	Server
OBJ +=	VirtualServer
OBJ +=	ServerRun
OBJ +=	ServerReceive
OBJ +=	ServerInit
OBJ +=	ServerAccept
OBJ +=	ServerRespond
OBJ +=	Client
OBJ +=	RequestHandler
OBJ +=	RequestHandlerMimeTypes
OBJ +=	RequestHandlerStatusCodes
OBJ +=	Authentication
OBJ +=	base64

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
