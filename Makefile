# ---------------------------------------------------------------------------
# Project settings
# ---------------------------------------------------------------------------
TARGET    := ca_server
BUILD_DIR := build

# Add new source files here (one per line).
SRCS := \
	main.c \
	src/network/server.c \
	src/network/connection.c \
	src/network/con_buffer.c

# Header search paths.
INC_DIRS := \
	include \
	include/network \
	include/network/common

# ---------------------------------------------------------------------------
# Toolchain and flags
# ---------------------------------------------------------------------------
CC       := gcc
CFLAGS   := -std=c11 -Wall -Wextra -Wpedantic -g
CPPFLAGS := $(addprefix -I,$(INC_DIRS)) -D_GNU_SOURCE -MMD -MP   # _GNU_SOURCE: expose Linux/POSIX extensions (e.g. SO_REUSEPORT)
LDFLAGS  :=
LDLIBS   :=            # e.g. -lssl -lcrypto

# ---------------------------------------------------------------------------
# Derived variables (no need to edit below this line)
# ---------------------------------------------------------------------------
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# Mirror the source tree inside $(BUILD_DIR), e.g. src/server.c -> build/src/server.o
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Auto-generated header dependencies (rebuild when a header changes).
-include $(DEPS)
