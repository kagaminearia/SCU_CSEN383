CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = project6

SOURCES = main.c utils.c common.c child1to4.c child_interactive.c child_monitor.c PipeSetup.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) output.txt

.PHONY: all clean