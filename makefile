include makefile.inc

PROCESS_SOURCES=$(wildcard *.c)
PROCESS_OBJECTS=$(PROCESS_SOURCES:.c=.o)

UTILS_SOURCES=$(wildcard ./utils/*.c)
UTILS_OBJECTS=$(UTILS_SOURCES:.c=.o)

BIN_DIR = ./bin
OUT = stegobmp


all: $(PROCESS_OBJECTS) $(UTILS_OBJECTS)
	$(CC) $(CFLAGS) $(PROCESS_OBJECTS) $(UTILS_OBJECTS) -o $(BIN_DIR)/$(OUT) $(LDFLAGS) 


%.o: %.c
	$(CC) $(CFLAGS) -I./include -c $< -o $@ $(LDFLAGS) 
	mkdir -p bin

clean:
	rm -f $(PROCESS_OBJECTS) $(UTILS_OBJECTS)
	rm -rf $(BIN_DIR)
.PHONY: all clean