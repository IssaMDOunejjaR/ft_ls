EXEC			= ft_ls

LIB_NAME	= lib.a
LIB_SRCS	= $(filter-out ./lib/test.c, $(wildcard ./lib/*.c))
LIB_OBJS	= $(LIB_SRCS:.c=.o)
	
SRCS			= $(wildcard ./utils/*.c)
OBJS			= $(SRCS:.c=.o)
	
FLAGS			= -Wall -Wextra -Werror -I ./headers

all: $(EXEC)

$(EXEC): lib $(OBJS)
	gcc main.c ./libcft/ds/list/ft_list.a ./libcft/string/ft_string.a $(OBJS) $(LIB_NAME) -o $(EXEC) 

lib: $(LIB_OBJS)
	ar rcs $(LIB_NAME) $(LIB_OBJS)

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -rf ./**/*.o

fclean: clean
	rm -rf $(NAME)

re: fclean all
