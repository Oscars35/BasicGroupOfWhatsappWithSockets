#include<stdio.h>	
#include<string.h> 
#include<stdlib.h>
#include<stdbool.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<signal.h>
#include<unistd.h>

#define DEFAULT_PORT 1234
#define DEFAULT_HOST "127.0.0.1"
#define BUFFER_SIZE 1024

#define COLOR_RED "\033[01;31m"
#define COLOR_END "\033[00m"

struct sockaddr_in socketStructure;
int clientSocket, childPid, slen=sizeof(socketStructure);

void die(char *s);
void initializeSocketConfiguration();
void connectServer();
void readInfoAndSendInfo();
void readFromServer();
void readFromKeyboardAndSendServer();
void exitMenuHandler(int sig);
void sendNickNameToServer();


int main(void) 
{
    initializeSocketConfiguration();
    connectServer();
    sendNickNameToServer();
    readInfoAndSendInfo();
	close(clientSocket);
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
    bzero(&socketStructure, sizeof(socketStructure));
    socketStructure.sin_family = AF_INET; 
    socketStructure.sin_addr.s_addr = inet_addr(DEFAULT_HOST); 
    socketStructure.sin_port = htons(DEFAULT_PORT);
}

void connectServer() {
    if(connect(clientSocket, (struct sockaddr *) &socketStructure, sizeof(socketStructure)) < 0)
        die("Error connect()");
}

void readInfoAndSendInfo() {
    childPid = fork();
    if (childPid == 0) {
        readFromServer();
    }else{
        if (signal(SIGINT, exitMenuHandler) == SIG_ERR)
            perror("Error initializing SIGINT");
        readFromKeyboardAndSendServer();
    }

}

void sendNickNameToServer() {
    char nickName[BUFFER_SIZE];
    printf("Enter your nickName: ");
    fgets(nickName, sizeof(nickName), stdin);
    if(sendto(clientSocket, nickName, strlen(nickName), 0 , (struct sockaddr *) &socketStructure, slen) < 0)
        die("sendto()");
}

void readFromServer() {
	char buf[BUFFER_SIZE];
	while(true)
	{
        memset(buf,'\0', BUFFER_SIZE); 
        if (recv(clientSocket, buf, sizeof(buf), 0) <= 0) {
            kill(getppid(), SIGKILL);
            die("Error reciving from server: Recv()");
        }
        printf("\n%s\n", buf);
	}
}

void readFromKeyboardAndSendServer(){
	char message[BUFFER_SIZE];
    while(true)
    {
        memset(message, '\0', BUFFER_SIZE);
        fgets(message, sizeof(message), stdin);
        if (sendto(clientSocket, message, strlen(message) , 0 , (struct sockaddr *) &socketStructure, slen) < 0)
			      die("sendto()");
    }
}

// Handle ctrl + c exit
void exitMenuHandler(int sig) {
            printf("%s\nAre you sure to exit? y|n %s \n", COLOR_RED, COLOR_END);
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


