TARGET_EXEC ?= a.out

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
CC=gcc

SRCS= \
	src/wak_lang/lexer.c \
	src/wak_lang/parser.c \
	src/wak_lang/tail_parser.c \
	src/wak_lang/token.c \
	src/wak_lib/file.c \
	src/wak_lib/timers.c \
	src/wak_lib/vector.c 

#SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS ?= -foptimize-sibling-calls -O2 -Iinclude -MMD -MP -march=native -Werror=implicit -Wall -Wextra -Wno-unused-parameter -save-temps -foptimize-sibling-calls
LDFLAGS ?= -foptimize-sibling-calls -O2 -march=native -Werror=implicit -Wall -Wextra -Wno-unused-parameter -save-temps -foptimize-sibling-calls


$(BUILD_DIR)/test: $(OBJS) $(BUILD_DIR)/tests/lex_speed.c.o
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/wak_lang: $(OBJS) $(BUILD_DIR)/src/wak_lang/main.c.o
	$(CC) $^ -o $@ $(LDFLAGS)

# c source
$(BUILD_DIR)/%.c.o: %.c
	echo $@
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@



.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(OBJS:.o=.d)
