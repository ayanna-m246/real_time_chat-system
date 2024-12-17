# real_time_chat-system
Overview
The Multi-Client Chat Application allows a server to handle multiple clients simultaneously. Each client can:

Send messages to the server.
Receive targeted responses from the server.
The server:

Maintains a list of active clients.
Allows the server operator to select a specific client for communication.
Logs all messages in a chat_history.txt file with timestamps.
Features
Two-Way Communication:

Both server and client can send and receive messages in real-time.
Multi-Client Support:

The server handles multiple clients concurrently using threads.
Targeted Messaging:

The server can choose which client to send a message to.
Message Logging:

All messages (from clients and the server) are logged in chat_history.txt with timestamps.
File Structure
server.cpp: Server-side code.
client.cpp: Client-side code.
socketutil.h: Utility functions for socket creation and configuration.
chat_history.txt: Log file where all messages are recorded.
Setup and Compilation
Dependencies:
GCC or G++ compiler.
A Linux or WSL environment for socket programming.
Compile the Code:
Compile the server:

g++ server.cpp -o server -pthread

Compile the client:

g++ client.cpp -o client -pthread

How to Run
Step 1: Start the Server
Run the server in one terminal:

./server

The server will display:

Socket was bound successfully!
Server is listening on port 3000

Step 2: Start Clients
Open separate terminals for each client and run:


./client

Each client will display:


Connected to the server!

Step 3: Communicate

Clients can send messages to the server.

The server will:
Show a list of active clients.
Prompt the operator to select a client to respond to.
Send the message to the selected client.
