#include<stdio.h>	
#include<string.h> 
#include<stdlib.h>
#include<stdbool.h>
#include<arpa/inet.h>
#include<sys/socket.h>


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
	struct sockaddr_in si_other; 
	int s, slen=sizeof(si_other);
	char buf[BUFFER_SIZE];
	char message[BUFFER_SIZE];

	if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}
  
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(DEFAULT_PORT);
	
	if (inet_aton(DEFAULT_HOST , &si_other.sin_addr) == 0) 
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	if (sendto(s, CONNECTING_MESSAGE, strlen(CONNECTING_MESSAGE) , 0 , (struct sockaddr *) &si_other, slen)==-1)

    {
        die("sendto()");
    }

    int pid = fork();
    if (pid == 0) {
	    while(1)
	    {
            memset(buf,'\0', BUFFER_SIZE);
            
            int n = recvfrom(s, buf, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &si_other, &slen) == -1;
            if (n == -1)
            {
                die("recvfrom()");
            }
            if (!strcmp("1", buf) == 0)
                printf("Message arrived from another person: %s \n", buf);
            else
                printf("Conected to the server! yay!\n");
    
	    }
    } else {
		    printf("Enter messages to send : \n");
        while(1)
        {
		        gets(message);

		        if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
		        {
			       die("sendto()");
		        }
        }
    }


	close(s);
	return 0;
}
