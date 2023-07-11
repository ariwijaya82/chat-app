#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#define SERVER_IP "127.0.0.1"
#define PORT 8888
#define MAX_BUFFER_SIZE 1024
#define SEND_INTERVAL 5 // in seconds
#define NUM_CLIENTS 5

int main() {
    int sockfd[NUM_CLIENTS];
    struct sockaddr_in server_addr;
    char buffer[NUM_CLIENTS][MAX_BUFFER_SIZE];

    // Create sockets for each client
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        if ((sockfd[i] = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("Failed to create socket");
            return 1;
        }
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Failed to configure server address");
        return 1;
    }

    while (true) {
        for (int i = 0; i < NUM_CLIENTS; ++i) {
            // Create an array of numbers
            int numbers[] = {i+1, i+2, i+3, i+4, i+5};

            // Convert the array to a char buffer
            memcpy(buffer[i], numbers, sizeof(numbers));

            // Send the array to the server
            if (sendto(sockfd[i], buffer[i], sizeof(numbers), 0,
                       (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                perror("Failed to send data");
                break;
            }

            std::cout << "Sent array of numbers from client " << i+1 << " to server" << std::endl;
        }

        // Sleep for the specified interval
        std::this_thread::sleep_for(std::chrono::seconds(SEND_INTERVAL));
    }

    for (int i = 0; i < NUM_CLIENTS; ++i) {
        close(sockfd[i]);
    }

    return 0;
}
