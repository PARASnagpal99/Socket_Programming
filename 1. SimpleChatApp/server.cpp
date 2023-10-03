#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    // Check for the correct number of command-line arguments
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port_number>\n", argv[0]);
        exit(1);
    }

    // Initialize socket descriptors and other variables
    int sockfd, newsockfd, portno;
    int n;
    char buffer[255];

    // Define socket address structures for server and client
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    // Create a socket (IPv4, TCP)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("Error opening socket");
    }

    // Clear and set server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Binding failed");
    }

    // Listen for incoming connection requests
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // Accept the connection request from the client
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
    {
        error("Error on accept");
    }

    while (1)
    {
        // Clear the buffer
        memset(buffer, 0, sizeof(buffer));

        // Read data from the client
        n = read(newsockfd, buffer, sizeof(buffer));
        if (n < 0)
        {
            error("Error on reading");
        }

        printf("Client: %s\n", buffer);

        // Clear the buffer
        memset(buffer, 0, sizeof(buffer));

        // Read input from the console
        printf("You: ");
        fgets(buffer, sizeof(buffer), stdin);

        // Write the response back to the client
        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0)
        {
            error("Error on writing");
        }

        // Check for termination condition
        int i = strncmp("Bye", buffer, 3);
        if (i == 0)
        {
            break;
        }
    }

    // Close the socket connections
    close(newsockfd);
    close(sockfd);

    return 0;
}
