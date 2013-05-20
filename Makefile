CC = clang
CFLAGS += -Wall -g
LDFLAGS += -lreadline

OBJS = ngsh.o builtin_cd.o builtin_echo.o builtin_exit.o builtin_export.o builtin_history.o lex.yy.o

ngsh : ${OBJS}

ngsh.o : ngsh.h lex.yy.h builtin.h
builtin_cd.o builtin_echo.o builtin_export.o : builtin.h
builtin_exit.o builtin_history.o : builtin.h ngsh.h
lex.yy.o : ngsh.h lex.yy.h
lex.yy.h lex.yy.c : lexer.l
	flex $<

.PHONY : clean
clean :
	$(RM) ngsh ${OBJS} lex.yy.h lex.yy.c

