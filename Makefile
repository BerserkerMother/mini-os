SRC_DIR := src
BUILD_DIR := build

CC := i686-elf-gcc

C_FILES := $(wildcard $(SRC_DIR)/*.c)
S_FILES := $(wildcard $(SRC_DIR)/*.s)
FORMATABLES := $(C_FILES) $(S_FILES)

C_OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_FILES))
S_OBJECTS := $(patsubst $(SRC_DIR)/%.s, $(BUILD_DIR)/%.o, $(S_FILES))
OBJECTS := $(C_OBJECTS) $(S_OBJECTS)

# formating
FORMAT_STYLE := LLVM

.PHONY: all compile clean format

# default
all: format compile run 

$(BUILD_DIR): 
	@echo "making build dir"
	mkdir -p $@

format:
	@echo "formatting files..."
	clang-format -i --style $(FORMAT_STYLE) $(FORMATABLES)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c 
	@echo "[CC] $<"
	$(CC) -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	@echo "[AS] $<"
	$(CC) -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

link: $(OBJECTS)
	@echo "[LD] linking os..."
	$(CC) -T src/linker.ld -o $(BUILD_DIR)/os.bin -ffreestanding -O2 -nostdlib $(wildcard $(BUILD_DIR)/*.o) -lgcc

compile: $(BUILD_DIR) $(OO_FILES) link

run:
	@echo "[RUN]"
	qemu-system-i386 -kernel $(BUILD_DIR)/os.bin

clean:
	@echo "[CLEAN] removing artifacts..."
	rm -r $(BUILD_DIR)
