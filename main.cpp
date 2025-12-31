#include <iostream>
#include <string>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

struct sockaddr_in server_address;
int server_socket, client_socket, opt = 1, server_address_length = sizeof(server_address);
string receivedData;

bool recv_all(int sock) {
	while (receivedData.find("\r\n\r\n") == string::npos) {
        char chunk[1024];
		size_t received = recv(sock, chunk, 1024, 0); 
        if (received == 0) break;
        else if (received < 0) {
            return false;
        }
		receivedData += chunk;
	}
	return true;
}

int main() {
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (server_socket < 0) {
		perror("server socket init failed");
		return -1;
	}

	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(8000);
	inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

	int server_socket_bind = bind(server_socket, (struct sockaddr *)&server_address, server_address_length);
	if (server_socket_bind < 0) {
		perror("server socket bind failed");
		return -1;
	}

    cout << "Listening..." << endl;

    listen(server_socket, 1);

	client_socket = accept(server_socket, (struct sockaddr *)&server_address, (socklen_t *)&server_address_length);
	if (client_socket < 0) {
		perror("accept failed");
		close(server_socket);
		return -1;
	}

    cout << "Connection claimed" << endl;

    cout << "Receiving data.." << endl;

    if (recv_all(client_socket)) {
        cout << "Well done!" << endl;
        cout << receivedData << endl;
    } else cout << "Failed!" << endl;

    close(server_socket);

    return 0;
}