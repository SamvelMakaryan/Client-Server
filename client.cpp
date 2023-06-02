#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

constexpr const int PORT = 8080;
constexpr const char *SERVER_IP = "127.0.0.1";
constexpr const int BUFFER_SIZE = 1024;

void receiveMessages(int clientSocket) {
    char buffer[BUFFER_SIZE] = {0};
    int valRead;
    while (true) {
        valRead = read(clientSocket, buffer, BUFFER_SIZE);
        if (valRead <= 0) {
            std::cerr << "Connection terminated by server." << std::endl;
            break;
        }
        std::cout << buffer << std::endl;
        memset(buffer, 0, BUFFER_SIZE);
    }
    close(clientSocket);
}

int main() {
	std::string nic;
	std::cout << "Enter your name\n";
	std::cin >> nic;
    int clientSocket;
    struct sockaddr_in serverAddr;
    const char *message;
    std::string userInput;
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation failed." << std::endl;
        return -1;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported." << std::endl;
        return -1;
    }
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed." << std::endl;
        return -1;
    }
    if (fork() == 0) {
        receiveMessages(clientSocket);
        return 0;
    }
    std::cout << "Connected to the server. Start typing messages ('exit' to quit):" << std::endl;
    while (true) {
		std::cout << nic << " : ";
        std::getline(std::cin, userInput);
		userInput += '\n';
		userInput = '\n' + nic + " : " + userInput;
        message = userInput.c_str();
        send(clientSocket, message, strlen(message), 0);
        if (userInput == "exit") {
            std::cout << "Connection terminated by " << nic << std::endl;
            break;
        }
    }
    close(clientSocket);
    return 0;
}

