/*server.c*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define MAX_MESSAGE_SIZE 1024

// Structure to store client information
struct ClientInfo {
    int socket;
    struct sockaddr_in address;
};

// Function to handle client connections
void *handleClient(void *arg) {
    struct ClientInfo *clientInfo = (struct ClientInfo *)arg;
    char message[MAX_MESSAGE_SIZE];

    while (1) {
        ssize_t bytes_received = recv(clientInfo->socket, message, sizeof(message), 0);
        if (bytes_received == -1) {
            perror("Receive failed");
        } else if (bytes_received == 0) {
            printf("Client disconnected\n");
            break;
        } else {
            // Null-terminate the received data
            message[bytes_received] = '\0';

            // Echo the message back to the client
            send(clientInfo->socket, message, strlen(message), 0);
            printf("Received from %s:%d: %s", inet_ntoa(clientInfo->address.sin_addr), ntohs(clientInfo->address.sin_port), message);
        }
    }

    // Close the client socket
    close(clientInfo->socket);
    free(clientInfo);
    pthread_exit(NULL);
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr;
    socklen_t client_addr_len = sizeof(struct sockaddr_in);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        // Accept a connection
        struct ClientInfo *clientInfo = (struct ClientInfo *)malloc(sizeof(struct ClientInfo));
        clientInfo->socket = accept(server_socket, (struct sockaddr *)&clientInfo->address, &client_addr_len);
        if (clientInfo->socket == -1) {
            perror("Accept failed");
            free(clientInfo);
            continue;
        }

        // Create a new thread to handle the client
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handleClient, clientInfo) != 0) {
            perror("Failed to create client thread");
            free(clientInfo);
            close(clientInfo->socket);
        }
    }

    // Close the server socket (this should never be reached in this example)
    close(server_socket);

    return 0;
}
