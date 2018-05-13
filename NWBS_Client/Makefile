CC=gcc
LD=gcc
CFLAGS=-c
LDFLAGS=
OBJS=nwbs_errno.o nwbs_handler.o nwbs_menu.o nwbs_lib.o main.o
ELF=nwbs_client
RM=rm -rf
SO=-l pthread

$(ELF):$(OBJS)
	@$(LD) $(LDFLAGS) $(OBJS) $(SO) -o $@

main.o:main.c
	@$(CC) $(CFLAGS) $<

%.o:%.c %.h
	@$(CC) $(CFLAGS) $<

clean:
	@$(RM) $(OBJS)