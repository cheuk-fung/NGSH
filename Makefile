CC = clang
CFLAGS += -Wall -g
LDFLAGS += -lreadline

ngsh : ngsh.o lex.yy.o
ngsh.o: ngsh.h lex.yy.h
lex.yy.h lex.yy.c : lexer.l
	flex $<
lex.yy.o: ngsh.h

.PHONY : clean
clean :
	$(RM) ngsh.o lex.yy.o lex.yy.h lex.yy.c ngsh

