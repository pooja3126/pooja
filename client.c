/*client.c*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define MAX_MESSAGE_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char message[MAX_MESSAGE_SIZE];
    int message_counter = 0;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    while (1) {
        // Construct the "ping" message
        snprintf(message, sizeof(message), "Ping message #%d\n", ++message_counter);

        // Send the message to the server
        send(client_socket, message, strlen(message), 0);

        // Receive and display the server's response
        ssize_t bytes_received = recv(client_socket, message, sizeof(message), 0);
        if (bytes_received == -1) {
            perror("Receive failed");
        } else if (bytes_received == 0) {
            printf("Server disconnected\n");
            break;
        } else {
            // Null-terminate the received data
            message[bytes_received] = '\0';
            printf("Server says: %s", message);
        }

        // Sleep for 1 second
        sleep(1);
    }

    // Close the client socket
    close(client_socket);

    return 0;
}
