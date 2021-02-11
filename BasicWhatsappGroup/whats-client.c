#include<stdio.h>	//printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<stdbool.h>
#include<arpa/inet.h>
#include<sys/socket.h>

//Variables globals per a que quedi més bonic
#define DEFAULT_PORT 1234
#define DEFAULT_HOST "127.0.0.1"
#define BUFFER_SIZE 1024
char *CONNECTING_MESSAGE = "1";

void die(char *s) 
{
	perror(s);
	exit(1);
}

int main(void) 
{
	struct sockaddr_in si_other; //Struct of sockets
	int s, i, slen=sizeof(si_other);
	char buf[BUFFER_SIZE];
	char message[BUFFER_SIZE];
	
	//If creation of sockets fails
	if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(DEFAULT_PORT);
	
	// If is an error copiing to sin_addr, show 
	if (inet_aton(DEFAULT_HOST , &si_other.sin_addr) == 0) 
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	if (sendto(s, CONNECTING_MESSAGE, strlen(CONNECTING_MESSAGE) , 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
        	die("sendto()");
        }


	while(1)
	{
		//receive a reply and print it
    		//clear the buffer by filling null, it might have previously received data
    		memset(buf,'\0', BUFFER_SIZE);
   		 //try to receive some data, this is a blocking call
    		int n = recvfrom(s, buf, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &si_other, &slen) == -1;
    		if (n == -1)
    		{
     			die("recvfrom()");
    		}
    
    		printf("Message arrived from another person: %s \n", buf);

		printf("Enter message to send : ");
		gets(message);
		
		//send the message
		if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
		{
			die("sendto()");
		}
		
	}
	close(s);
	return 0;
}
