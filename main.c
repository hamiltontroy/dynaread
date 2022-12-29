#include <unistd.h>         // read() getpagesize() close() write()
#include <stdlib.h>         // malloc() realloc() free()
#include <netinet/in.h>     // sockaddr_in
#include <sys/socket.h>     // socket bind listen accept
#include <string.h>         // memset
#include <stdio.h>          // printf

struct pfixbuf
{
    long buflen;
    void *buf;
};

int dynaread(int fd, const long max, struct pfixbuf *infobuf)
{
    unsigned char
        *buffer, *temp_pointer;
    ssize_t 
        PAGE_SIZE, page_increments, size_after_realloc, bytes_read = 0, total_bytes = 0;
    
    (*infobuf).buflen = 0; // initial setup
    (*infobuf).buf = NULL; // initial setup
    
    PAGE_SIZE = (size_t)getpagesize();
    if(max < PAGE_SIZE)
    {
        PAGE_SIZE = max;
    }
    
    buffer = (unsigned char*)malloc(PAGE_SIZE); // malloc an initial buffer
    if(buffer == NULL)
        return -1;
    
    page_increments = 1;
    size_after_realloc = PAGE_SIZE;
    
    while(total_bytes < max)
    {                                               //assures no overflow
        bytes_read = read(fd, buffer + total_bytes, size_after_realloc - total_bytes);
        if(bytes_read < 0)
        {
            free(buffer);
            return -1;
        }
        
        if(bytes_read == 0) // breaks if reading is finished
        {
            break;
        }
        
        total_bytes += bytes_read;
        if(total_bytes > max - 1) // stops if the max limit is reached
        {
            break;
        }
        
        // checks if the buffer got full, reallocs then continues if necessary
        if(bytes_read == PAGE_SIZE)
        {
            page_increments++;
            size_after_realloc = page_increments * PAGE_SIZE;
            if(size_after_realloc > max)
            {
                size_after_realloc = max;
            }
            
            temp_pointer = (unsigned char*)realloc(buffer, size_after_realloc);
            if(temp_pointer == NULL)
            {
                free(buffer);
                return -1;
            }
            
            buffer = temp_pointer;
        }
        else
            break;
    }
    
    //resize buffer to a smaller size.
    temp_pointer = (unsigned char*)realloc(buffer, total_bytes);
    if(temp_pointer == NULL)
    {
        free(buffer);
        return -1;
    }
    
    buffer = temp_pointer;
    
    (*infobuf).buflen = (long)total_bytes; // note to self "(*type).member" is the same as "type -> member"
    (*infobuf).buf = (void*)buffer;
    
    return 0;
}


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

void printslen(const char *str, int slen)
{
    for(int i = 0; i < slen; i++)
    {		
        putchar(*(str + i));
    }
    putchar('\n');
}

void passThrough(int clientSocket)
{
    struct pfixbuf
        infobuf;
    
    if(dynaread(clientSocket, 8192, &infobuf) == -1)
        printError("dynaread", __FILE__, __LINE__);
    
    printf("%ld\n", infobuf.buflen);
    printslen((char *)infobuf.buf, infobuf.buflen);
}

int main(void)
{
    int
        serverSocket, clientSocket;

    serverSocket = listeningTcpSocket(1111);
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