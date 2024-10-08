EXEC			= ft_ls

LIB_PATH	= ./libcft
	
SRCS			= $(wildcard ./utils/*.c)
OBJS			= $(SRCS:.c=.o)
	
FLAGS			= -Wall -Wextra -Werror -I ./headers

all: $(EXEC)

$(EXEC): lib $(OBJS)
	gcc main.c $(LIB_PATH)/ds/list/ft_list.a $(LIB_PATH)/string/ft_string.a $(LIB_PATH)/number/ft_number.a $(LIB_PATH)/io/ft_io.a $(OBJS) -o $(EXEC) 

lib:
	cd ./libcft && $(MAKE) ds string number io

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -rf ./**/*.o

fclean: clean
	rm -rf $(NAME)

re: fclean all
