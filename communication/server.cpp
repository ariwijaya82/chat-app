#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8888
#define MAX_BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failed to create socket");
        return 1;
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Bind socket to the server address
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to bind socket");
        return 1;
    }

    while (true) {
        std::cout << "testing" << std::endl;
        memset(buffer, 0, sizeof(buffer));

        // Receive data from client
        int recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                (struct sockaddr*)&client_addr, &client_len);
        if (recv_len < 0) {
            perror("Failed to receive data");
            break;
        }

        // Print received data
        std::cout << "Received data from client: " << buffer << std::endl;

        // Process received data (assuming it contains an array of integers)
        int* numbers = reinterpret_cast<int*>(buffer);
        int count = recv_len / sizeof(int);
        std::cout << "Received " << count << " numbers:" << std::endl;
        for (int i = 0; i < count; ++i) {
            std::cout << numbers[i] << " ";
        }
        std::cout << std::endl;
    }

    close(sockfd);
    return 0;
}
