ARGS = -Wall -ansi -pedantic

client server: client.c server.c
	gcc ${ARGS} -o client client.c
	gcc ${ARGS} -o server server.c

clean: 
	rm client server