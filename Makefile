NAME = ft_ls
SRCS = utils/options.c \
			 utils/params.c \
			 utils/errors.c \
			 utils/printing.c
OBJS = $(SRCS:.c=.o)
FLAGS = -Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(OBJS)
	gcc main.c $(OBJS) lib/lib.a -o $(NAME) 

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -rf *.o
	rm -rf utils/*.o

fclean: clean
	rm -rf $(NAME)

re: fclean all
