#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define PORT 15635
#define BUFFER_SIZE 1024

/* 
    Test File Sizes 
        test.html   449
        test.pdf    69609
        test.txt    21s
        test.jpg    387448
        test.png    3418725
*/

// TODO: set socket to reuse

int main(int argc, char const* argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = { 0 };
 
    // 1. Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        int err = errno;
        perror("socket() failed");
        exit(err);
    }
 
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
 
    // 2. Forcefully attaching socket to the port 15635
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        int err = errno;
        perror("bind() failed");
        exit(err);
    }

    // 3. Listen for connections with the server
    if (listen(server_fd, 3) < 0) {
        int err = errno;
        perror("listen() failed");
        exit(err);
    }

    // 4. Accept a connection from a client, typically blocks until a client connects with the server
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        int err = errno;
        perror("accept() failed");
        exit(err);
    }

    // 5. Read client request
    valread = read(new_socket, buffer, BUFFER_SIZE); // reads from accept() return value, store in buffer
    if (valread == -1){
        int err = errno;
        perror("read() failed");
        exit(err);
    }

    // Parse request
    char requestType[BUFFER_SIZE] = {0};
    char requestFile[BUFFER_SIZE] = ".";
    char contentType[BUFFER_SIZE] = {0};
    char tmp[BUFFER_SIZE] = {0};
    const char *requestFileType;

    sscanf(buffer, "%s %s", requestType, tmp);
    strncat(requestFile, tmp, BUFFER_SIZE);
    requestFileType = strrchr(requestFile, '.') + 1;

    printf("%s %s %s\n", requestType, requestFile, requestFileType);

    if (strcmp(requestType, "GET") != 0){
        int err = errno;
        perror("incorrect request type");
        exit(err);
    }

    if (strcmp(requestFileType, "html") == 0){
        strncpy(contentType, "text/html; charset=utf-8", BUFFER_SIZE);
    } else if (strcmp(requestFileType, "txt") == 0){
        strncpy(contentType, "text/plain; charset=utf-8", BUFFER_SIZE);
    } else if (strcmp(requestFileType, "pdf") == 0){
        strncpy(contentType, "application/pdf", BUFFER_SIZE);
    } else if (strcmp(requestFileType, "jpg") == 0){
        strncpy(contentType, "image/jpg", BUFFER_SIZE);
    } else if (strcmp(requestFileType, "png") == 0){
        strncpy(contentType, "image/png", BUFFER_SIZE);
    }

    // Open requested file
    FILE *fptr;
    unsigned char *dataBuffer;
    long filelen;

    fptr = fopen(requestFile, "rb"); // OPEN fptr
    if (fptr == NULL) { 
        int err = errno;
        perror("fopen failed");
        exit(err);
    }

    // Get size of file
    fseek(fptr, 0L, SEEK_END);
    filelen = ftell(fptr);
    rewind(fptr);

    // Read the file
    dataBuffer = (unsigned char *)malloc((filelen + 1) * sizeof(char));
    int bytes_read = fread(dataBuffer, 1, filelen, fptr);
    dataBuffer[filelen] = '\0';
    int dataBufferLen = sizeof(&dataBuffer)*sizeof(dataBuffer[0]);

    fclose(fptr); // CLOSE fptr

    // 6. Generate and send HTTP response
    time_t t; 
    time(&t);
    char date[30];
    strftime(date, 30, "%a, %d %b %Y %T %Z", gmtime(&t));

    size_t response_len = snprintf(NULL, 0, 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Date: %s\r\n"
        "\r\n%s", contentType, dataBufferLen, date, dataBuffer);
    char* response = (char *)malloc(response_len + 1);
    sprintf(response, 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Date: %s\r\n"
        "\r\n%s", contentType, dataBufferLen, date, dataBuffer);
    if (send(new_socket, response, response_len, 0) == -1) {
        int err = errno;
        perror("send() failed");
        exit(err);
    }
    printf("%ld %d %d %zu\n", filelen, dataBufferLen, bytes_read, response_len);
    printf("%s\n", response);
    
    free(dataBuffer); // Free the memory
    close(new_socket); // closing the connected socket
    shutdown(server_fd, SHUT_RDWR); // closing the listening socket
    return 0;
}