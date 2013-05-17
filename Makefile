CC = clang
CFLAGS += -Wall -g
LDFLAGS += -lreadline

ngsh: ngsh.o

.PHONY : clean
clean :
	$(RM) ngsh.o ngsh

