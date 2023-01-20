
OBJECT += $(patsubst $(ARCH_DIR)%.s, $(OBJ_DIR)%.o, $(wildcard $(ARCH_DIR)*.s))
OBJECT += $(patsubst $(ARCH_DIR)%.c, $(OBJ_DIR)%.o, $(wildcard $(ARCH_DIR)*.c))

$(OBJ_DIR)%.o: $(ARCH_DIR)%.s
	$(COMPILER)-as -c $< -o $@

$(OBJ_DIR)%.o: $(ARCH_DIR)%.c
	$(COMPILER)-gcc $(CFLAGS) -c $< -o $@
