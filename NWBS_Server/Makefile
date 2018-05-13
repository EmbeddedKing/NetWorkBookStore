CC=gcc
LD=gcc
CFLAGS=-c
LDFLAGS=
OBJS=nwbs_errno.o nwbs_handler.o nwbs_lib.o main.o
RM=rm -rf
ELF=nwbs_server
SO=-lmysqlclient -lpthread

$(ELF):$(OBJS)
	@$(LD) $(LDFLAGS) $(OBJS) $(SO) -o $@

main.o:main.c
	@$(CC) $(SO) $(CFLAGS) $<

%.o:%.c %.h
	@$(CC) $(SO) $(CFLAGS) $<

clean:
	@$(RM) $(OBJS)