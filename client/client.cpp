#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

using namespace std;


int main() {
    const int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        cerr << "Socket creation error" << endl;
        return -1;
    }

    struct sockaddr_in server = {};
    server.sin_family = AF_INET;
    server.sin_port = htons(7432);

    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0) {
        cerr << "Invalid address/ Address not supported" << endl;
        return -1;
    }

    if (connect(sock, reinterpret_cast<sockaddr *>(&server), sizeof(struct sockaddr_in)) < 0) {
        cerr << "Connection Failed" << endl;
        return -1;
    }

    cout << "Client connected" << endl;
    
    string buffer;
    char buff[1024];
    bool isDisconnect = false;
    while (!isDisconnect) {
        cout << "Enter SQL query: ";
        getline(cin, buffer);

        if (buffer == "EXIT") {
            isDisconnect = true;
            send(sock, buffer.c_str(), buffer.size(), 0);
            continue;
        }

        send(sock, buffer.c_str(), buffer.size(), 0);
        ssize_t recvLen = recv(sock, &buff, sizeof buff, 0);

        if (recvLen == -1) {
            cerr << "Read failed: " << strerror(errno) << endl;
            break;
        }

        if (recvLen == 0) {
            cerr << "Server closed the connection" << endl;
            break;
        }

        buff[recvLen] = '\0';
        cout << buff << endl;
    }

    close(sock);
    return 0;
}