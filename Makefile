OBJS := shell.c builtins.c 

CFLAGS_ARGS := -Wall
CFLAGS_ARGS_DEBUG := -g -Wall
CFLAGS_ARGS_WINDOWS := -g -Wall -mwindows 

BIN := RYSCS

LIBS :=

CC := gcc
CC_WIN := winegcc

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

windows:
	@echo "compiling for windows"
	${CC_WIN} ${OBJS} ${CFLAGS_ARGS_WINDOWS} -o ${BIN} ${LIBS}
