# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: matheme <matheme@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/10/03 11:06:26 by yalabidi          #+#    #+#              #
#    Updated: 2022/12/02 14:10:01 by matheme          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


BLUE=\033[0;38;5;123m
LIGHT_PINK = \033[0;38;5;200m
PINK = \033[0;38;5;198m
DARK_BLUE = \033[1;38;5;110m
GREEN = \033[1;32;111m
LIGHT_GREEN = \033[1;38;5;121m
LIGHT_RED = \033[0;38;5;110m
FLASH_GREEN = \033[33;32m
WHITE_BOLD = \033[37m

# nom de l'executable
NAME = vox

#sources path
SRC_PATH= srcs

#objects path
OBJ_PATH= .objs

#includes
INC_PATH= includes/

HEADER= $(INC_PATH)/*

#framework
FRAMEWORK= -lglfw -framework opengl -L libraries/lib/glfw3lib -lfreetype -L libraries/lib/freetypelib
FRAMEWORK_INC = libraries/include

NAME_SRC=	camera.cpp\
			chunk.cpp\
			cubeMap.cpp\
			cubeMesh.cpp\
			debug.cpp\
			entity.cpp\
			event.cpp\
			init.cpp \
			main.cpp \
			minecraft.cpp\
			motor.cpp\
			perlin_noise.cpp\
			shader.cpp\
			simplex_noise.cpp\
			textDisplay.cpp\
			texture.cpp\
			thread.cpp\
			VAO.cpp\
			VBO.cpp\
			

NAME_SRC_C=glad.c \

NAME_SRC_LEN	= $(shell echo -n $(NAME_SRC) | wc -w)
I				= 0

OBJ_NAME		= $(NAME_SRC:.cpp=.o)

OBJ_NAME_C		= $(NAME_SRC_C:.c=.o)

OBJS = $(addprefix $(OBJ_PATH)/,$(OBJ_NAME)) $(addprefix $(OBJ_PATH)/,$(OBJ_NAME_C))

OPTIMISATION_FLAG = -ofast #-fsanitize=address#-o3 #-ofast  pas d'interet pour l'instant

CC			= gcc $(OPTIMISATION_FLAG)
GPP			= g++ -std=c++11 $(OPTIMISATION_FLAG)



all: install $(NAME)

$(NAME) : $(OBJS)
	@$(GPP) $^ -o $@ $(FRAMEWORK)
	@echo "	\033[2K\r$(DARK_BLUE)$(NAME):\t\t$(GREEN)loaded\033[0m"

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp $(HEADER) Makefile
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	@$(GPP) -I $(INC_PATH) -I $(FRAMEWORK_INC) -c $< -o $@
	@$(eval I=$(shell echo $$(($(I)+1))))
	@printf "\033[2K\r${G}$(DARK_BLUE)>>\t\t\t\t$(I)/$(shell echo $(NAME_SRC_LEN)) ${N}$(BLUE)$<\033[36m \033[0m"

$(OBJ_PATH)/%.o: %.c $(HEADER) Makefile
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	@$(CC) -I $(FRAMEWORK_INC) -c $< -o $@


clean:
ifeq ("$(wildcard $(OBJ_PATH))", "")
else
	@rm -f $(OBJS)
	@rmdir $(OBJ_PATH) 2> /dev/null || true
	@printf "\033[2K\r$(DARK_BLUE)$(NAME) objects:\t$(LIGHT_PINK)removing\033[36m \033[0m\n"
endif


fclean: clean
ifeq ("$(wildcard $(NAME))", "")
else
	@rm -f $(NAME)
	@printf "\033[2K\r$(DARK_BLUE)$(NAME):\t\t$(PINK)removing\033[36m \033[0m\n"
endif

re: fclean all


test: all
	@./vox

install:
	@sh install.sh

.PHONY: all re clean fclean lib silent
