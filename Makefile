OBJS := shell.c 

CFLAGS_ARGS := 
CFLAGS_ARGS_DEBUG := -g

BIN := RYSCS

LIBS :=

CC := gcc

install:
	@echo "Compiling"
	${CC} ${OBJS} ${CFLAGS_ARGS} -o ${BIN} ${LIBS}


#compiling to debug with gdb
debug:
	@echo "compiling binaries with gdb debuging information"
	${CC} ${OBJS} ${CFLAGS_ARGS_DEBUG} -o ${BIN} ${LIBS}

clean:
	@echo "Cleaning up"
	rm -rf ${BIN} 
