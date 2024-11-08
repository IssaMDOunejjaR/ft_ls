.PHONY: lib
	
EXEC			= ft_ls
LIB_PATH	= ./libcft
SRCS			= $(wildcard ./utils/*.c) $(wildcard ./main.c)
OBJS			= $(SRCS:.c=.o)
FLAGS			= -Wall -Wextra -Werror

all: $(EXEC)

install:
	sudo install -v $(EXEC) /usr/bin

$(EXEC): lib $(OBJS)
	gcc $(FLAGS) $(OBJS) $(LIB_PATH)/ds/list/ft_list.a $(LIB_PATH)/string/ft_string.a $(LIB_PATH)/number/ft_number.a $(LIB_PATH)/io/ft_io.a $(LIB_PATH)/table/ft_table.a -o $(EXEC) 

lib:
	cd ./libcft && $(MAKE) ds string number io table

%.o: %.c
	gcc $(FLAGS) -c $< -o $@

clean:
	rm -rf ./**/*.o

fclean: clean
	rm -rf $(EXEC)

re: fclean all
