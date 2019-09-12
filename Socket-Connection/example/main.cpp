#include "../library/fsocket.hpp"
#include <iostream>
#include <windows.h>

static const std::string server_ip = "127.0.0.1";
static const unsigned int server_port = 5555;

// ** SERVER ** //

bool server_handler(SOCKET connection, fsocket::Packet packet) {
	if (packet == fsocket::Packet::TestInt) {
		int data;
		bool flag1 = fsocket::Net::recvInt(connection, data);
		if (!flag1)
			return false;
		printf("Received TestInt packet from Client. Value: %i\n", data);

		bool flag2 = fsocket::Net::sendPacket(connection, fsocket::Packet::TestInt);
		if (!flag2)
			return false;

		bool flag3 = fsocket::Net::sendInt(connection, 228);
		if (!flag3)
			return false;
	}
	else if (packet == fsocket::Packet::TestString) {
		std::string data;
		bool flag1 = fsocket::Net::recvString(connection, data);
		if (!flag1)
			return false;
		printf("Received TestString packet from Client. Value: %s\n", data.c_str());

		bool flag2 = fsocket::Net::sendPacket(connection, fsocket::Packet::TestString);
		if (!flag2)
			return false;

		bool flag3 = fsocket::Net::sendString(connection, "Hi, i'm Server!");
		if (!flag3)
			return false;
	} else {
		printf("Received unknown packet from Client\n");
	}
}

void server_thread() {
	fsocket::Server* server = new fsocket::Server(server_ip, server_port, server_handler);
	if (!server->start())
		return;

	printf("[Server] Initialized!\n");
}

// ** CLIENT ** //

bool client_handler(SOCKET connection, fsocket::Packet packet) {
	if (packet == fsocket::Packet::TestInt) {
		int data;
		bool flag1 = fsocket::Net::recvInt(connection, data);
		if (!flag1)
			return false;
		printf("Received TestInt packet from Server. Value: %i\n", data);
	} else if (packet == fsocket::Packet::TestString) {
		std::string data;
		bool flag1 = fsocket::Net::recvString(connection, data);
		if (!flag1)
			return false;
		printf("Received TestString packet from Server. Value: %s\n", data.c_str());
	} else {
		printf("Received unknown packet from Client\n");
	}
}

void client_thread() {
	fsocket::Client* client = new fsocket::Client(server_ip, server_port, client_handler);
	if (!client->start())
		return;

	printf("[Client] Initialized!\n");

	bool flag1 = fsocket::Net::sendPacket(client->serverSocket, fsocket::Packet::TestInt);
	if (!flag1)
		return;
	bool flag2 = fsocket::Net::sendInt(client->serverSocket, 1337);
	if (!flag2)
		return;

	bool flag3 = fsocket::Net::sendPacket(client->serverSocket, fsocket::Packet::TestString);
	if (!flag3)
		return;
	bool flag4 = fsocket::Net::sendString(client->serverSocket, "Hi, i'm Client!");
	if (!flag4)
		return;
}

int main() {
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)server_thread, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)client_thread, 0, 0, 0);

	while (1);

	return 0;
}