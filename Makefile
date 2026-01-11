NAME = raylib-ui

LIBS =  -L./lib -lraylib -lm
FLAGS = -Wall -Wextra
SANITIZE = -fsanitize=address -fno-omit-frame-pointer -static-libsan

INC = -Iinclude -I. -Itests

SRC=raylib-ui.c private-helper.c
TEST_SRC = test_find_element.c test_helper.c testmain.c
EXAMPLE_SRC = example.c

VPATH=tests

OBJ_DIR = obj/

OBJ = $(addprefix $(OBJ_DIR), $(patsubst %.c,%.o,$(SRC)))
TEST_OBJ = $(addprefix $(OBJ_DIR), $(patsubst %.c,%.o,$(TEST_SRC)))
EXAMPLE_OBJ = $(addprefix $(OBJ_DIR), $(patsubst %.c,%.o,$(EXAMPLE_SRC)))


all: libs example test

libs: $(OBJ)
	gcc $(FLAGS) -shared -g $(INC) $(filter %.o,$^) -o $(NAME).so $(LIBS)
	ar cr lib$(NAME).a $(filter %.o,$^)

$(OBJ_DIR)%.o: %.c
	gcc $(FLAGS) $(INC) -g -c $< -o $@

example: libs $(EXAMPLE_OBJ)
	gcc $(FLAGS) $(INC) -g $(EXAMPLE_OBJ) -o $@ $(LIBS) -L. -lraylib-ui

re: clean all

test: libs $(TEST_OBJ) $(OBJ)
	gcc $(FLAGS) -g $(INC) $(TEST_OBJ) $(OBJ) -o testrunner $(LIBS)


clean:
	- rm -f $(OBJ)
	- rm -f lib$(NAME).a
	- rm -f $(NAME).so
	- rm -f example
	@echo "object directory cleaned!"

fclean:
	rm -f $(OBJ)
	rm -f $(NAME)
	@echo "fully cleaned!"