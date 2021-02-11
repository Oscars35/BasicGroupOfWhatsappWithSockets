#include<stdio.h>	
#include<string.h> 
#include<stdlib.h>
#include<stdbool.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<signal.h>

#define DEFAULT_PORT 1234
#define DEFAULT_HOST "127.0.0.1"
#define BUFFER_SIZE 1024

char *CONNECTING_MESSAGE = "1";
struct sockaddr_in si_other;
int clientSocket, childPid, slen=sizeof(si_other);

void die(char *s);
void initializeSocketConfiguration();
void sendConnectMessage();
void readInfoAndSendInfo();
void readFromServer();
void readFromKeyboardAndSendServer();
void exitMenuHandler(int sig);
void emptyHandler(int sig);


int main(void) 
{
    initializeSocketConfiguration();
    sendConnectMessage();
    readInfoAndSendInfo();
	pclose(clientSocket);
	return 0;
}

void die(char *s) 
{
	perror(s);
	exit(1);
}

void initializeSocketConfiguration() {
	if ( (clientSocket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("socket");
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(DEFAULT_PORT);
	
	if (inet_aton(DEFAULT_HOST , &si_other.sin_addr) == 0) 
        die("inet_aton() failed \n");
}

void sendConnectMessage() {
	if (sendto(clientSocket, CONNECTING_MESSAGE, strlen(CONNECTING_MESSAGE) , 0 , (struct sockaddr *) &si_other, slen)==-1)
        die("sendto()");
}

void readInfoAndSendInfo() {
    childPid = fork();
    if (childPid == 0) {
        if (signal(SIGINT, emptyHandler) == SIG_ERR)
            perror("Error initializing SIGUSR1");
        readFromServer();
    }else{
        if (signal(SIGINT, exitMenuHandler) == SIG_ERR)
            perror("Error initializing SIGINT");
        readFromKeyboardAndSendServer();
    }

}

void readFromServer() {
	char buf[BUFFER_SIZE];
	while(true)
	{
        memset(buf,'\0', BUFFER_SIZE);
        if(recvfrom(clientSocket, buf, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &si_other, &slen) == -1)
            die("recvfrom()");
        if (!strcmp("1", buf) == 0)
            printf("Message arrived from another person: %s \n", buf);
        else
            printf("Conected to the server! yay!\n");
	}
}

void readFromKeyboardAndSendServer(){
    printf("Enter messages to send : \n");
    while(true)
    {
	    char message[BUFFER_SIZE];
        fgets(message, sizeof(message), stdin);
		if (sendto(clientSocket, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
			die("sendto()");
    }
}

// Handle ctrl + c exit
void exitMenuHandler(int sig) {
        printf("\nAre you sure to exit? y|n \n");
        char option;
        do {
            option = getchar();
        } while(option != 'y' && option != 'n');
        if (option == 'y') {
            kill(childPid, SIGKILL);
            exit(0);
        }
        if (signal(SIGINT, exitMenuHandler) == SIG_ERR)
            perror("Error initializing SIGUSR1");
}

// handle ctrl + c for kid
void emptyHandler(int sig) {
    if (signal(SIGINT, emptyHandler) == SIG_ERR)
        perror("Error initializing SIGUSR1");
}

