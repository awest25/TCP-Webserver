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

    // parse request
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
    unsigned char *data_buffer;
    long filelen;

    if ((fptr = fopen(requestFile, "rb")) == NULL) { // OPEN fptr
        int err = errno;
        perror("fopen failed");
        exit(err);
    }

    // Get the length of the file
    fseek(fptr, 0, SEEK_END);
    filelen = ftell(fptr);
    rewind(fptr);

    // Allocate memory for the buffer
    data_buffer = (unsigned char *)malloc((filelen + 1) * sizeof(char));

    // Read the entire file
    size_t bytes_read = 0;
    while(bytes_read < filelen) {
        bytes_read += fread(data_buffer + bytes_read, 1, filelen - bytes_read, fptr);
    }

    fclose(fptr); // CLOSE fptr

    // Add a null character to the end of the buffer
    data_buffer[filelen] = '\0';

    // 6. Generate and send HTTP response
    size_t response_len = snprintf(NULL, 0, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n%s", contentType, filelen, data_buffer);
    char* response = (char *)malloc(response_len + 1);
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n%s", contentType, filelen, data_buffer);
    if (send(new_socket, response, response_len, 0) == -1) {
        int err = errno;
        perror("send() failed");
        exit(err);
    }
    printf("Response message sent: %s\n", response);
    
    free(data_buffer); // Free the memory
    close(new_socket); // closing the connected socket
    shutdown(server_fd, SHUT_RDWR); // closing the listening socket
    return 0;
}