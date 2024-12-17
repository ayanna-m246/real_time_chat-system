#include "socketutil.h"
#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <atomic>

//receiving messages from the server
void receiveMessages(int socketFD,atomic<bool> &running) {
    char buffer[1024];
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            cout << "Disconnected from server!" << endl;
            running = false;
            close(socketFD);
            break;
        }
        cout << "Server: " << buffer << endl;
    }
}

//sending messages to the server
void sendMessages(int socketFD, atomic<bool> &running) {
    string message;
    while (running) {
        getline(cin, message);
        if (message == "exit") {
            running = false;
            close(socketFD);
            break;
        }
        send(socketFD, message.c_str(), message.size(), 0);
    }
}

int main() {
    int socketFD = createTCPIpv4Socket();

    struct sockaddr_in *address = createSocketAddress("127.0.0.1", 3000);
    if (connect(socketFD, (struct sockaddr *)address, sizeof(*address)) == -1) {
        perror("Connection failed");
        return -1;
    }
    cout << "Connected to the server!" <<endl;

    atomic<bool> running(true);

    // Start threads for two-way communication
    thread recvThread(receiveMessages, socketFD, ref(running));
    thread sendThread(sendMessages, socketFD, ref(running));

    recvThread.join();
    sendThread.join();

    delete address;

    return 0;
}

