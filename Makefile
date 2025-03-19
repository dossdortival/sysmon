CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lncurses

SRC = src/main.c src/ui.c src/system_stats.c src/process.c
OBJ = $(SRC:.c=.o)

all: sysmon

sysmon: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f sysmon $(OBJ)


