CC = clang
CFLAGS += -g
LDFLAGS += -lreadline

ngsh: ngsh.o

.PHONY : clean
clean :
	$(RM) ngsh.o ngsh

