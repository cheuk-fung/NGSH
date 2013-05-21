CC = clang
CFLAGS += -Wall -g
LDFLAGS += -lreadline

PRGS = ngsh getenv
all : ${PRGS}

NGSH_OBJS = ngsh.o builtin.o builtin_cd.o builtin_echo.o builtin_exit.o builtin_export.o builtin_history.o lex.yy.o
ngsh : ${NGSH_OBJS}
ngsh.o : ngsh.h lex.yy.h builtin.h
builtin_cd.o builtin_echo.o builtin_export.o : builtin.h
builtin_exit.o builtin_history.o : builtin.h ngsh.h
lex.yy.o : ngsh.h lex.yy.h
lex.yy.h lex.yy.c : lexer.l
	flex $<

getenv : getenv.o

.PHONY : clean
clean :
	$(RM) ${PRGS} ${NGSH_OBJS} lex.yy.h lex.yy.c getenv.o
