#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> //sleep func
#include <future>
#include <mutex>
#include <thread>
#include <arpa/inet.h>
#include "../libs/database.h"

mutex dbMutex;

void requestProcessing(const int clientSocket, const sockaddr_in& clientAddress, string baseName){
    char userMessage[1024] = {};
    bool isExit = false;

    while (!isExit){
        bzero(userMessage, 1024);
        const ssize_t userRead = read(clientSocket, userMessage, 1024);

        if (userRead <= 0){
            cerr << "Сlient [" << clientAddress.sin_addr.s_addr << "] disconnected" << endl;
            isExit = true;
            continue;
        }

        if (string(userMessage) == "EXIT"){
            cerr << "Сlient [" << clientAddress.sin_addr.s_addr << "] disconnected" << endl;
            isExit = true;
            continue;
        }

        string result;
        {
            lock_guard<std::mutex> guard(dbMutex);
            result = database(userMessage, baseName);
        }

        send(clientSocket, result.c_str(), result.size(), 0);
    }
    close(clientSocket);
}

void startServer(string baseName){
    const int server = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server == -1){
        cerr << "Socket creation error" << endl;
        return;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(7432);

    if (bind(server, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) < 0){
        cerr << "Binding error" << endl;
        return;
    }
    
    if (listen(server, 1) == -1){
        cerr << "Socket listening error" << endl;
        return;
    }

    cout << "Server started" << endl;

    sockaddr_in clientAddress{};
    socklen_t clientAddLen = sizeof(clientAddress);

    while (true){
        int clientSocket = accept(server, reinterpret_cast<struct sockaddr *>(&clientAddress), &clientAddLen);
        if (clientSocket == -1){ 
            cout << "Connecting fail" << endl;
            continue;
        }

        cout << "Client [" << clientAddress.sin_addr.s_addr << "] was connected" << endl;

        thread(requestProcessing, clientSocket, clientAddress, baseName).detach();
    }

    close(server);
    cout << "Server stopped." << endl;
}

int main(){
    string baseName;
    readJson(baseName);

    cout << "Database ready" << endl;

    startServer(baseName);

    return 0;
}