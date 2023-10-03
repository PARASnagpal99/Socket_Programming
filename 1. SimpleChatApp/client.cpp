/*
argv[0] -> file name 
argv[1] -> server_ipaddress
argv[2] -> port number 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[255];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[2]);

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening socket");
    }

    // Get host information by hostname or IP address
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Error: No such host\n");
        exit(1);
    }

    // Clear and set the server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("Connection failed");
    }

    while (1) {
        memset(buffer, 0, sizeof(buffer));

        // Read user input from stdin
        printf("You: ");
        fgets(buffer, sizeof(buffer), stdin);


        // Send the user input to the server
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            error("Error on writing");
        }

        memset(buffer, 0, sizeof(buffer));

        // Read the server's response
        n = read(sockfd, buffer, sizeof(buffer));
        if (n < 0) {
            error("Error on reading");
        }

        printf("Server: %s\n", buffer);

        int i = strncmp("Bye", buffer, 3);
        if (i == 0) {
            break;
        }
    }

    // Close the socket
    close(sockfd);
    return 0;
}
