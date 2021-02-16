#include<stdio.h>	
#include<string.h> 
#include<stdlib.h>
#include<stdbool.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<signal.h>
#include<unistd.h>
#include<pthread.h>

#define DEFAULT_PORT 1234
#define DEFAULT_HOST "127.0.0.1"
#define BUFFER_SIZE 1024

#define COLOR_RED "\033[01;31m"
#define COLOR_END "\033[00m"

struct sockaddr_in socketStructure;
int clientSocket, slen=sizeof(socketStructure);
pthread_t inputThread;
pthread_t serverThread;

void die(char *s);
void initializeSocketConfiguration();
void connecserverThread();
void readInfoAndSendInfo();
void* readFromServer(void* arg);
void* readFromKeyboardAndSendServer(void* arg);
void exitMenuHandler(int sig);
void sendNickNameToServer();
void clearInputBuffer();


int main(void) {
    initializeSocketConfiguration();
    connecserverThread();
    sendNickNameToServer();
    readInfoAndSendInfo();
	close(clientSocket);
	return 0;
}

void die(char *s) {
	perror(s);
	exit(1);
}

void initializeSocketConfiguration() {
	if ( (clientSocket=socket(AF_INET, SOCK_STREAM, 0)) < 0)
		die("socket()");
    memset(&socketStructure, 0, sizeof(socketStructure));
    socketStructure.sin_family = AF_INET; 
    socketStructure.sin_addr.s_addr = inet_addr(DEFAULT_HOST); 
    socketStructure.sin_port = htons(DEFAULT_PORT);
}

void connecserverThread() {
    if(connect(clientSocket, (struct sockaddr *) &socketStructure, sizeof(socketStructure)) < 0)
        die("Error connect()");
}

void readInfoAndSendInfo() {
    if (signal(SIGINT, exitMenuHandler) == SIG_ERR)
        perror("Error initializing SIGINT");
    if (pthread_create(&serverThread, NULL, &readFromServer, NULL) != 0)
        perror("Error creating thread reads from server");
    if (pthread_create(&inputThread, NULL, &readFromKeyboardAndSendServer, NULL) != 0)
        perror("Error creating thread reads from keyboard");
    if (pthread_join(serverThread, NULL) != 0)
        perror("Error Waiting thread read from server");
    if (pthread_join(inputThread, NULL) != 0)
        perror("Error waiting thread read from keyboard");
}

void sendNickNameToServer() {
    char nickName[BUFFER_SIZE];
    printf("Enter your nickName: ");
    fgets(nickName, sizeof(nickName), stdin);
    if(sendto(clientSocket, nickName, strlen(nickName), 0 , (struct sockaddr *) &socketStructure, slen) < 0)
        die("sendto()");
}

void* readFromServer(void* arg) {
	char buf[BUFFER_SIZE];
	while(true)
	{
        memset(buf,'\0', BUFFER_SIZE); 
        if (recv(clientSocket, buf, sizeof(buf), 0) <= 0) {
            die("Error reciving from server: Recv()");
        }
        printf("\n%s\n", buf);
	}
}

void* readFromKeyboardAndSendServer(void* arg){
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
        pthread_cancel(serverThread);
        pthread_cancel(inputThread);
        printf("%s\nAre you sure to exit? y|n %s \n", COLOR_RED, COLOR_END);
        char option;
        do {
            option = getchar();
        } while(option != 'y' && option != 'n');
        clearInputBuffer();
        if (option == 'y')
            exit(0);
        if (signal(SIGINT, exitMenuHandler) == SIG_ERR)
            perror("Error initializing SIGUSR1");
        readInfoAndSendInfo();
}

void clearInputBuffer() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}
