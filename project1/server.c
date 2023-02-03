#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define PORT 15635
#define BUFFER_SIZE 1024
#define SPACE "\%20"
#define SPACE_LEN 3

int new_socket, server_fd;

/* 
    Test File Sizes 
        test.html   449
        test.pdf    69609
        test.txt    21
        test.jpg    387448
        test.png    3418725

    Requirements
        jpg                 done
        png                 done
        pdf                 done
        txt                 done
        html                done
        binary              done
        ext with spaces     done
        ext with %          done  
        ext with .          done
        binary with spaces  done
        binary with %       done

    curl -o downloadfile localhost:15635/binary%20file

*/

void sigHandler(int signum){
    fprintf(stderr, "\nserver.c: Caught signal %d, exiting safely\n", signum);
    shutdown(server_fd, SHUT_RDWR); // closing the listening socket
    close(new_socket); // closing the connected socket'
    close(server_fd);
    exit(0); 
}

char* replaceSpace(const char* s, int* numSpaceReplaced){
    int i = 0;
    *numSpaceReplaced = 0;
    char* result;

    // search for occurances of %20 in the string
    for (i = 0; s[i] != '\0'; i++) { 
        if (strstr(&s[i], SPACE) == &s[i]) { 
            (*numSpaceReplaced)++; 
            i += SPACE_LEN - 1; 
        } 
    } 
    result = (char*)malloc(i + (*numSpaceReplaced) + (SPACE_LEN - 1) + 1);
    if (result == NULL) exit(errno);

    // replace all occurrances of %20 with " "
    i = 0;
    while (*s) {
        if (strstr(s, SPACE) == s){
            strcpy(&result[i], " ");
            i += 1;
            s += SPACE_LEN;
        } else {
            result[i++] = *s++;
        }
    }
    result[i] = '\0';
    return result;
}

int main(int argc, char const* argv[])
{
    if (signal(SIGINT, sigHandler) == SIG_ERR) {
        fprintf(stderr, "Error setting signal handler for signal %d: %s\n", SIGINT, strerror(errno));
        exit(errno);
    }
    if (signal(SIGTERM, sigHandler) == SIG_ERR) {
        fprintf(stderr, "Error setting signal handler for signal %d: %s\n", SIGTERM, strerror(errno));
        exit(errno);
    }
    int valread;
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
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
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
    while (1) {
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
        char *requestFileType;
        int numSpaceReplaced = 0;

        // get file extension
        if (sscanf(buffer, "%s %s", requestType, tmp) < 0) exit(errno);
        strncat(requestFile, tmp, BUFFER_SIZE);
        requestFileType = strrchr(requestFile, '.') + 1;

        // handle binary files
        if (strcmp(tmp, requestFileType) == 0){
            requestFileType = "binary";
        }

        // handle files with white space
        strncpy(requestFile, replaceSpace(requestFile, &numSpaceReplaced), BUFFER_SIZE);
        if (numSpaceReplaced != 0 && strcmp(requestFileType, "binary") != 0){
            requestFileType -= (numSpaceReplaced)*SPACE_LEN - 1;
        }

        // printf("=========\n");
        // printf("spaces: %d\n", numSpaceReplaced);
        // printf("requested: %s\n", requestFile);
        // printf("type: %s\n", requestFileType);

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
        } else if (strcmp(requestFileType, "binary") == 0){
            strncpy(contentType, "application/octet-stream", BUFFER_SIZE);
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
        if (fseek(fptr, 0L, SEEK_END) != 0) exit(errno);
        filelen = ftell(fptr);
        if (filelen == -1L) exit(errno);
        rewind(fptr);

        // Read the file
        dataBuffer = (unsigned char *)malloc((filelen) * sizeof(char));
        int bytes_read = fread(dataBuffer, 1, filelen, fptr);
        dataBuffer[filelen] = '\0';

        fclose(fptr); // CLOSE fptr

        // 6. Generate and send HTTP response
        time_t t; 
        time(&t);
        char date[30];
        strftime(date, 30, "%a, %d %b %Y %T %Z", gmtime(&t));

        size_t response_len = snprintf(NULL, 0, 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n"
            "Date: %s\r\n"
            // "Connection: close\r\n" // i added this but it does nothing i believe
            "\r\n", contentType, filelen + 1, date); // need \r\n at the end
        
        char* response = (char *)malloc(response_len + filelen); // + 1 or 2?
        sprintf(response, 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n"
            "Date: %s\r\n"
            // "Connection: close\r\n"
            "\r\n", contentType, filelen, date);
        memcpy(response + response_len, dataBuffer, filelen);
        if (send(new_socket, response, response_len + filelen + 1, 0) == -1) { // + 1 or 2?
            int err = errno;
            perror("send() failed");
            exit(err);
        }

        printf("%s\n", response);    
        free(dataBuffer); // Free the memory
        printf("\nserver.c: Waiting for a request...\n\n");
    }
    // should not reach this area
    printf("TCP socket not closed correctly");
    return 0;
}