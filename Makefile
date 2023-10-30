# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: matheme <matheme@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/10/03 11:06:26 by yalabidi          #+#    #+#              #
#    Updated: 2023/01/26 19:10:34 by matheme          ###   ########.fr        #
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
FRAMEWORK= -ldl -lpthread `pkg-config --static --libs gl glfw3` -L ~/.dep/usr/lib/x86_64-linux-gnu/ -lopenal -lsndfile
FRAMEWORK_INC = -I libraries/include -I libraries/imgui/ -I libraries/imgui/backends

NAME_SRC=	camera.cpp\
			chunk.cpp\
			cubeMap.cpp\
			cubeMesh.cpp\
			menu.cpp\
			sound.cpp\
			entity.cpp\
			event.cpp\
			init.cpp \
			main.cpp \
			generation.cpp \
			world_area.cpp\
			perlin_noise.cpp\
			shader.cpp\
			simplex_noise.cpp\
			texture.cpp\
			thread.cpp\
			VAO.cpp\
			VBO.cpp\
			parseConfig.cpp\
			raycast.cpp\
			application.cpp\
			background.cpp\

IMGUI_DIR = libraries/imgui/

IMGUI_SOURCES = $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
IMGUI_SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
IMGUI_OBJS = $(addsuffix .o, $(basename $(notdir $(IMGUI_SOURCES))))

CXXFLAGS = -std=c++11 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
CXXFLAGS += -g -Wall -Wformat
CXX = g++
			

NAME_SRC_C=glad.c \

NAME_SRC_LEN	= $(shell echo -n $(NAME_SRC) | wc -w)
I				= 0

OBJ_NAME		= $(NAME_SRC:.cpp=.o)

OBJ_NAME_C		= $(NAME_SRC_C:.c=.o)

OBJS = $(addprefix $(OBJ_PATH)/,$(OBJ_NAME)) $(addprefix $(OBJ_PATH)/,$(OBJ_NAME_C)) $(addprefix $(OBJ_PATH)/,$(IMGUI_OBJS))

DEBUG_FLAG = -Wall -Wextra #-fsanitize=address

OPTIMISATION_FLAG = -ofast #-fsanitize=address#-o3 #-ofast  pas d'interet pour l'instant

CC			= gcc $(OPTIMISATION_FLAG)
GPP			= g++ -std=c++20 $(OPTIMISATION_FLAG) $(DEBUG_FLAG)



all: $(NAME)

$(NAME) : $(OBJS)
	@$(GPP) $^ -o $@ $(FRAMEWORK)
	@echo "	\033[2K\r$(DARK_BLUE)$(NAME):\t\t$(GREEN)loaded\033[0m"

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp $(HEADER) Makefile
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	@$(GPP) -I $(INC_PATH) $(FRAMEWORK_INC) -c $< -o $@
	@$(eval I=$(shell echo $$(($(I)+1))))
	@printf "\033[2K\r${G}$(DARK_BLUE)>>\t\t\t\t$(I)/$(shell echo $(NAME_SRC_LEN)) ${N}$(BLUE)$<\033[36m \033[0m"

$(OBJ_PATH)/%.o: %.c $(HEADER) Makefile
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	@$(CC) $(FRAMEWORK_INC) -c $< -o $@

$(OBJ_PATH)/%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_PATH)/%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<


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
	@./install.sh

.PHONY: all re clean fclean lib silent
