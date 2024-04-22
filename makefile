FILTER_OUT = vpn_client.c client_connection.c vpn_server.c server_connection.c
SHARED = $(filter-out $(FILTER_OUT), $(wildcard *.c))

CPPFLAGS = vpn.h
CFLAGS = -g
RELFLAGS = -DNDEBUG -O3
GD = gcc -ansi -pedantic-errors -Wall -Wextra

all:

client: vpn_client
vpn_client: $(SHARED) vpn_client.c client_connection.c
	@$(GD) $(CPPFLAGS) $(CFLAGS) $^ -o $@

server: vpn_server
vpn_server: $(SHARED) vpn_server.c server_connection.c 
	@$(GD) $(CPPFLAGS) $(CFLAGS) $^ -o $@
