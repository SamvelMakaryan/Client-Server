#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

constexpr const int PORT = 8080;
constexpr const int BUFFER_SIZE = 1024;

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE] = {0};
    int valRead;
    std::string message;
    while (true) {
        valRead = read(clientSocket, buffer, BUFFER_SIZE);
        if (valRead <= 0) {
            std::cerr << "Connection terminated by client." << std::endl;
            break;
        }
        std::cout << buffer << std::endl;
        std::cout << "Server: ";
        std::getline(std::cin, message);
		message += '\n';
		message = "\nServer : " + message;
        send(clientSocket, message.c_str(), message.length(), 0);
        if (message == "exit") {
            std::cout << "Connection terminated by server." << std::endl;
            break;
        }
        memset(buffer, 0, BUFFER_SIZE);
    }
    close(clientSocket);
}

int main() {
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    int opt = 1;
    int addrlen = sizeof(serverAddr);
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed." << std::endl;
        return -1;
    }
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        std::cerr << "Setsockopt failed." << std::endl;
        return -1;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Binding failed." << std::endl;
        return -1;
    }
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listening failed." << std::endl;
        return -1;
    }
    std::cout << "Server is running. Waiting for incoming connections..." << std::endl;
    while (true) {
        if ((newSocket = accept(serverSocket, (struct sockaddr *)&serverAddr, (socklen_t *)&addrlen)) < 0) {
            std::cerr << "Accept failed." << std::endl;
            return -1;
        }
        if (fork() == 0) {
            close(serverSocket); 
            handleClient(newSocket);
            return 0;
        }
        close(newSocket);
    }
    return 0;
}

