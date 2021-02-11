all: 
	python whats-server.py &
	gcc -Wall whats-client.c -o client 
	./client

test: 
	gcc -Wall whats-client.c -o client
	./client

clean:
	rm client

