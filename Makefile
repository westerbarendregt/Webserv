OS 			:= $(shell uname)
PHP_PATH 	:= $(shell which php-cgi)
NAME		:=	webserv
FLAGS		=	-Wall -Wextra -Werror -std=c++98 -pedantic

SANITIZE	?=	0
DEBUG		?=	0
LOG			?=	0
LOG_FILE	?=	0
LEAKS		?= 0

ifeq ($(SANITIZE), 1)
FLAGS+= -g -fsanitize=address
endif



ifeq ($(DEBUG), 1)
FLAGS+= -g
endif

OUT_LOG=

ifeq ($(LOG), 1)
FLAGS+=	-DLOG
endif

ifeq ($(LOG_FILE), 1)
OUT_LOG+=  2>&1 | tee webserv.log
endif

INCLUDES	:=	-Iincludes

SRC_DIR		:= 	srcs
OBJ_DIR		:=	objs
CONF_DIR 		:=	conf
TEST_DIR	:= ${PWD}/tests

WWW			:= ${PWD}/www
REPO		:= ${PWD}

ifeq ($(OS), Linux)
CGI_TESTER := ubuntu_cgi_tester
else
CGI_TESTER := cgi_tester
endif

OBJ =	main
OBJ +=	base64
OBJ +=	Cgi
OBJ +=	Client
OBJ +=	Logger
OBJ +=	RequestHandler
OBJ +=	RequestHandlerMimeTypes
OBJ +=	RequestHandlerStatusCodes
OBJ +=	ResponseHeaders
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


all: leaks_in $(NAME) config leaks_out

leaks_in:
ifeq ($(LEAKS), 1)
ifneq ($(SANITIZE), 1)
ifneq (, $(shell which leaks))
	patch $(SRC_DIR)/main.cpp leaks_in.diff -R
endif 
endif
endif

leaks_out:
ifeq ($(LEAKS), 1)
ifneq ($(SANITIZE), 1)
ifneq (, $(shell which leaks))
	patch $(SRC_DIR)/main.cpp leaks_out.diff
endif 
endif 
endif

$(NAME): $(OBJ)
	@$(foreach obj, $?, echo Linking $(notdir $(obj))$(NL))
	@$(CC) $(FLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER)
	@mkdir -p $(OBJ_DIR)/$(dir $*)
	@echo Compiling $(notdir $<)
	@$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	@$(RM) -r $(wildcard $(OBJ_DIR)) $(subst $(WWW)/upload_store/empty.txt,, $(wildcard $(WWW)/upload_store/*))

clean_config:
	@$(RM) -r $(wildcard $(CONF_DIR)/*.conf)

fclean:
	@$(MAKE) clean clean_config
	$(RM) $(wildcard $(NAME))
	$(RM) leaks

re:
	$(MAKE) fclean
	$(MAKE) all

run: $(NAME) config
	@./$(NAME) $(OUT_LOG) || true
config:
	@WWW=$(WWW) \
		REPO=$(REPO) \
		TEST_DIR=$(TEST_DIR) \
		CGI_TESTER=$(CGI_TESTER) \
		PHP_PATH=$(PHP_PATH) \
		./generate_config.pl

.PHONY: all clean fclean re
