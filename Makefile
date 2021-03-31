NAME		:=	webserv
FLAGS		=	-Wall -Wextra -Werror -std=c++98 -pedantic

DEBUG 		?=	0
LOG			?=	1

ifeq ($(DEBUG), 1)
FLAGS+= -g -fsanitize=address
endif

LOG_FILE=

ifeq ($(LOG), 1)
FLAGS+=	-DLOG
LOG_FILE+=  2>&1 | tee webserv.log
endif

INCLUDES	:=	-Iincludes

SRC_DIR		:= 	srcs
OBJ_DIR		:=	objs
CONF_DIR 		:=	conf

OBJ =	main
OBJ +=	Authentication
OBJ +=	base64
OBJ +=	Cgi
OBJ +=	Client
OBJ +=	Logger
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
	$(CC) $(FLAGS) $(OBJ) -g -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER)
	@mkdir -p $(OBJ_DIR)/$(dir $*)
	@echo Compiling $(notdir $<)
	@$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	@$(RM) -r $(wildcard $(OBJ_DIR))

clean_config:
	@$(RM) -r $(wildcard $(CONF_DIR)/*.conf)

fclean:
	@$(MAKE) clean clean_config
	$(RM) $(wildcard $(NAME))

re:
	$(MAKE) fclean
	$(MAKE) all config

run: $(NAME) config
	./$(NAME) $(LOG_FILE)
config:
	WWW=${PWD}/www REPO=${PWD} ./generate_config.pl

.PHONY: all clean fclean re
