CC = gcc
OBJ = src/args.o src/cmditem.o src/fileio.o src/whatis.o src/r2sh.o
CFLAGS = -Isrc -DR2SH_VERSION

default: $(OBJ)
	$(CC) $^ -o r2sh

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm src/*.o
