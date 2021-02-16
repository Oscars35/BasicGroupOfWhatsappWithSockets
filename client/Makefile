all: 
	python whats-server.py &
	gcc whats-client.c -ansi -std=c99 -pedantic -Wall -lpthread -o client
	./client

test: 
	gcc whats-client.c -ansi -std=c99 -pedantic -Wall -lpthread -o client
	./client

clean:
	rm client

