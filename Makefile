NAME = scop
SRCDIR = srcs/
OBJDIR = .obj/
INCDIR = includes/
SRCFILES =	main.c \
			matrix.c \
			vector.c \
			loop.c \
			init.c \
			parse_file.c \
			parse_data.c \
			utils.c \
			str_utils.c \
			event.c \
			draw.c \
			VAO.c \

INCFILES = scop.h stb_image.h
SRC = $(addprefix $(SRCDIR),$(SRCFILES))
INC = $(addprefix $(INCDIR),$(INCFILES))

OBJ = $(SRCFILES:.c=.o)
OBJS = $(addprefix $(OBJDIR),$(OBJ))

FLAG = -I includes #-I ~/.brew/Cellar/glew/2.2.0_1/include -I ~/.brew/Cellar/glfw/3.3.8/include 
LINK = -lglfw -lGLEW -lGL -lstb -lm # -L ~/.brew/Cellar/glew/2.2.0_1/lib -L ~/.brew/Cellar/glfw/3.3.8/lib

all: $(NAME)

$(OBJDIR)%.o: $(SRCDIR)%.c $(INC)
	@mkdir $(OBJDIR) 2> /dev/null || true
	gcc  -c $< -o $@ $(FLAG)

$(NAME):$(OBJS) $(INC)
	gcc -o $@ $(OBJS) $(LINK)
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all