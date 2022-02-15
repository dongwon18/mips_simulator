cc = gcc
CCFLAGS =

SRCS = main.c

TARGET = mips_sim

OBJS := $(patsubst %.c,%.o,$(SRCS))

all: $(TARGET)

%.o:%.c
			$(CC) $(CCFLAGS) $< -c -o $@ -lm

$(TARGET): $(OBJS)
			$(CC) $(CCFLAGS) $^ -o $@ -lm

.PHONY = clean
.PHONY = run

run: $(TARGET)
			./$(TARGET)

clean:
			rm -f $(OBJS) $(TARGET)
