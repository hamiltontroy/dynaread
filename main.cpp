#include <netinet/in.h> //sockaddr_in
#include <sys/socket.h> //socket bind listen accept
#include <unistd.h>     //close read write
#include <string.h>     //memset
#include <stdio.h>      //printf


// printError("functionName", __FILE__, __LINE__);
void printError(const char *functionName, const char *fileName, int lineNumber)
{
    printf("********************************************************************************\nThere was a runtime error:\n{\n");
    if(functionName != NULL)
        printf("    Function Name: \"%s\"\n", functionName);
    if(fileName != NULL)
        printf("    File Name: \"%s\"\n", fileName);
    if(lineNumber > 0)
        printf("    Approximate Line Number: \"%d\"\n", lineNumber);
    printf("}\n********************************************************************************\n\n");
}

int listeningTcpSocket(unsigned short portNumber)
{
    int
        serverSocket;
    struct sockaddr_in
        serverSocketAddress;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1)
    {
        printError("socket", __FILE__, __LINE__);
        return -1;
    }
    
    memset(&serverSocketAddress, 0, sizeof(struct sockaddr_in));
    
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_port = htons(portNumber);
    serverSocketAddress.sin_addr.s_addr = INADDR_ANY;
    
    //assigns the address to the socket
    if(bind(serverSocket, (const struct sockaddr*) &serverSocketAddress, 
        sizeof(struct sockaddr_in)) == -1)
    {
        close(serverSocket);
        printError("bind", __FILE__, __LINE__);
        return -1;
    }
    
    //sets the socket to listen mode.
    if(listen(serverSocket, 4096) == -1)
    {
        close(serverSocket);
        printError("listen", __FILE__, __LINE__);
        return -1;
    }
    
    return serverSocket;
}

void passThrough(int); // handles tls stuff and http parsing, returns a struct with http info
#include "passThrough.cpp"

int main(void)
{
    int
        serverSocket, clientSocket;

    serverSocket = listeningTcpSocket(1025);
    if(serverSocket == -1)
    {
        printError("serverSocket", __FILE__, __LINE__);
        return -1;
    }
    
    while(1)
    {
        clientSocket = accept(serverSocket, NULL, NULL);
        if(clientSocket == -1)
        {
            printError("accept", __FILE__, __LINE__);
            close(clientSocket);
            continue;
        }
        
        passThrough(clientSocket);
        close(clientSocket);
    }
    
    return 0;
}