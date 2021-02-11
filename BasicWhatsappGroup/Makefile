all: 
	python whats-server.py &
	gcc -Wall whats-client.c -o client 
	./client

exec: 
	./client

clean:
	rm client

