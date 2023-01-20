
OBJECT += $(patsubst $(LIB_DIR)math/%.c, $(OBJ_DIR)%.o, $(wildcard $(LIB_DIR)math/*.c))

$(OBJ_DIR)%.o: $(LIB_DIR)math/%.c
	$(COMPILER)-gcc $(CFLAGS) -c $< -o $@