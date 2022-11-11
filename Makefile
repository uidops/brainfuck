.POSIX:

CC ?= cc
CFLAGS ?= -O3 -flto -Wall -Wextra
TARGET = brainfuck
TEST ?= test.bf
RM ?= rm

.PHONY: clean test all

all: $(TARGET)

$(TARGET):
	$(CC) -o $(TARGET) $(TARGET).c $(CFLAGS)

clean:
	$(RM) $(TARGET)

test:
	./$(TARGET) $(TEST)
