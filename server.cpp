
#include "socketutil.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <map>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <atomic>
#include <chrono>


std::mutex fileMutex;
std::mutex clientMutex;

//map to track active clients (clientID->socketFD)
map<int, int> clientMap;

//log messages with timestamps
void logMessage(const string &message) {
    lock_guard<mutex> lock(fileMutex);
    ofstream logFile("chat_history.txt",ios::app);
    if (logFile.is_open()) {
        auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        logFile << "[" << std::ctime(&now) << "] " << message << std::endl;
        logFile.close();
    }
}

//single client
void handleClient(int clientSocketFD, int clientID) {
    char buffer[1024];
    atomic<bool> running(true);

    cout << "Client " << clientID << " connected!" <<endl;
    logMessage("Client " + to_string(clientID) + " connected!");

    //receiving messages from the client
    thread recvThread([&]() {
        while (running) {
            memset(buffer, 0, sizeof(buffer));
            ssize_t bytesRead = recv(clientSocketFD, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                cout << "Client " << clientID << " disconnected!" << endl;
                logMessage("Client " + to_string(clientID) + " disconnected!");
                running = false;
                break;
            }
            string message(buffer);
            cout << "Client " << clientID << ": " << message << endl;
            logMessage("Client " + to_string(clientID) + ": " + message);
        }
    });

    recvThread.join();

    close(clientSocketFD);
    {
        lock_guard<mutex> lock(clientMutex);
        clientMap.erase(clientID); 
    }
    cout << "Client " << clientID << " handler terminated." << endl;
}

int main() {
    int serverSocketFD = createTCPIpv4Socket();

    int enable = 1;
    setsockopt(serverSocketFD, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    struct sockaddr_in *serverAddress = createSocketAddress("", 3000);

    if (bind(serverSocketFD, (struct sockaddr *)serverAddress, sizeof(*serverAddress)) == -1) {
        perror("Bind failed");
        return -1;
    }
    cout << "Socket was bound successfully!" <<endl;

    if (listen(serverSocketFD, 10) == -1) {
        perror("Listen failed");
        return -1;
    }
    cout << "Server is listening on port 3000" <<endl;

    int clientID = 1;

    //accept clients
    thread acceptThread([&]() {
        while (true) {
            struct sockaddr_in clientAddress;
            socklen_t clientAddressSize = sizeof(clientAddress);
            int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, &clientAddressSize);
            if (clientSocketFD == -1) {
                perror("Accept failed");
                continue;
            }

            {
                lock_guard<mutex> lock(clientMutex);
                clientMap[clientID] = clientSocketFD;
            }

           thread clientThread(handleClient, clientSocketFD, clientID++);
            clientThread.detach();
        }
    });

    // Main loop for sending messages to clients
    while (true) {
        int targetID;
        string message;

        {
            lock_guard<mutex> lock(clientMutex);
            cout << "Active clients: ";
            for (const auto &entry : clientMap) {
                cout << entry.first << " ";
            }
            cout << endl;
        }

        cout << "Enter the client ID to send a message: ";
        cin >> targetID;

        int targetSocketFD;
        {
            lock_guard<mutex> lock(clientMutex);
            if (clientMap.find(targetID) == clientMap.end()) {
                cout << "Invalid client ID. Try again." << endl;
                continue;
            }
            targetSocketFD = clientMap[targetID];
        }

        cin.ignore();
        cout << "Enter your message: ";
        getline(cin, message);

        send(targetSocketFD, message.c_str(), message.size(), 0);
        logMessage("Server to Client " + to_string(targetID) + ": " + message);
        cout << "Message sent to Client " << targetID << "." << endl;
    }

    acceptThread.join();

    close(serverSocketFD);
    delete serverAddress;

    return 0;
}
