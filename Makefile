CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lncurses

SRC = src/main.c src/ui.c src/system_stats.c
OBJ = $(SRC:.c=.o)

all: sysmon

sysmon: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f sysmon $(OBJ)


