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
void connectServer();
void readInfoAndSendInfo();
void readFromServer();
void readFromKeyboardAndSendServer();
void exitMenuHandler(int sig);
void emptyHandler(int sig);


int main(void) 
{
    initializeSocketConfiguration();
    connectServer();
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
	if ( (clientSocket=socket(AF_INET, SOCK_STREAM, 0)) < 0)
		die("socket()");
    bzero(&si_other, sizeof(si_other));
    si_other.sin_family = AF_INET; 
    si_other.sin_addr.s_addr = inet_addr(DEFAULT_HOST); 
    si_other.sin_port = htons(DEFAULT_PORT);
}

void connectServer() {
    if(connect(clientSocket, (struct sockaddr *) &si_other, sizeof(si_other)) < 0)
        die("Error connect()");
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
        if (recv(clientSocket, buf, sizeof(buf), 0) < 0)
            die("Recv()");
        if (sizeof(buf) < 0)
            exit(-1);
        printf("New message: %s \n", buf);
	}
}

void readFromKeyboardAndSendServer(){
    printf("Enter messages to send : \n");
    while(true)
    {
	    char message[BUFFER_SIZE];
        fgets(message, sizeof(message), stdin);
        if (send(clientSocket, message, sizeof(message), 0) < 0)
            die("Send()");
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

