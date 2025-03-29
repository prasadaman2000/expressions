// C++ program to show the example of server application in
// socket programming
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>

#include "runtime.h"
#include "compiler.h"

class Server{
public:
    Server(int port);
    Server() = delete;

    void start();

private:
    int server_fd_;
    std::thread listen_thread_;
};

Server::Server(int port) {
    // creating socket
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    // specifying the address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // binding socket.
    bind(server_fd_, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));

    // listening to the assigned socket
    listen(server_fd_, 5);
}

void Server::start() {
    listen_thread_ = std::thread([this, server_fd_ = &server_fd_](){
        while(true) {
            std::cout << "waiting! \n";

            int clientSocket = accept(*server_fd_, nullptr, nullptr);
            char buffer[1024] = { 0 };
            recv(clientSocket, buffer, sizeof(buffer), 0);

            Compiler c;
            Environment e;

            Program p = c.compile(buffer, &e);
            float ans = p.execute(&e);

            char send_buffer[1024] = {0};
            std::cout << "ans is " << ans << '\n';
            sprintf(send_buffer, "%f", ans);

            send(clientSocket, send_buffer, strlen(send_buffer), 0);

            close(clientSocket);
        }
    });

    listen_thread_.join();
}

int main() {
    Server s(8080);
    s.start();
    return 0;
}