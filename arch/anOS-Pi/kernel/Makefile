
OBJECT += $(patsubst $(KERNEL_DIR)%.c, $(OBJ_DIR)%.o, $(wildcard $(KERNEL_DIR)*.c))

$(OBJ_DIR)%.o: $(KERNEL_DIR)%.c
	$(COMPILER)-gcc $(CFLAGS) -c $< -o $@