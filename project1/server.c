#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#define PORT 15635
int main(int argc, char const* argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
 
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
 
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);

    time_t t; // do we need any of this?
    time(&t);
    char date[30];
    strftime(date, 30, "%a, %d %b %Y %T %Z", gmtime(&t));



    // Open and read the file
    FILE *fptr;
    unsigned char *data_buffer;
    long filelen;

    if ((fptr = fopen("./hello.html", "rb")) == NULL) {
        printf("File not found\n");
        return 1;
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

    fclose(fptr);

    // Add a null character to the end of the buffer
    data_buffer[filelen] = '\0';

    // printf("File content: %s\n", data_buffer);




    size_t response_len = snprintf(NULL, 0, "HTTP/1.1 200 OK\r\nContent-Type: text/HTML\r\nContent-Length: %ld\r\n\r\n%s", filelen, data_buffer);
    char* response = (char *)malloc(response_len + 1);
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/HTML\r\nContent-Length: %ld\r\n\r\n%s", filelen, data_buffer);
    if (send(new_socket, response, response_len, 0) == -1) {
        printf("Error sending response\n");
        perror("send");
        exit(1);
    }
    printf("Response message sent: %s\n", response);
    
    //Free the memory
    free(data_buffer);
 
    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}