/*
	Created by Fals3R
		30.08.19

	Simple header only socket server-client library
*/

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")

namespace fsocket {

	enum Packet {
		TestString,
		TestInt
	};

	namespace Net {
		bool sendBytes(SOCKET connection, char* data, int data_size) {
			int sent = 0;
			while (sent < data_size) {
				int bytes = send(connection, data + sent, data_size - sent, NULL);
				if (bytes == SOCKET_ERROR || bytes == 0)
					return false;
				sent += bytes;
			}
			return true;
		}

		bool recvBytes(SOCKET connection, char* data, int data_size) {
			int received = 0;
			while (received < data_size) {
				int bytes = recv(connection, data + received, data_size - received, NULL);
				if (bytes == SOCKET_ERROR || bytes == 0)
					return false;
				received += bytes;
			}
			return true;
		}

		bool sendInt(SOCKET connection, int data) {
			data = ntohl(data);
			if (!sendBytes(connection, (char*)& data, sizeof(int)))
				return false;
			return true;
		}

		bool recvInt(SOCKET connection, int& data) {
			if (!recvBytes(connection, (char*)& data, sizeof(int)))
				return false;
			data = ntohl(data);
			return true;
		}

		bool sendString(SOCKET connection, std::string data) {
			int str_length = data.size();
			if (!sendInt(connection, str_length))
				return false;
			if (!sendBytes(connection, (char*)data.c_str(), str_length))
				return false;
			return true;
		}

		bool recvString(SOCKET connection, std::string& data) {
			int str_length = 0;
			if (!recvInt(connection, str_length))
				return false;
			char* buffer = new char[str_length + 1];
			buffer[str_length] = '\0';
			if (!recvBytes(connection, buffer, str_length)) {
				delete[] buffer;
				return false;
			}
			data = buffer;
			delete[] buffer;
			return true;
		}

		bool sendPacket(SOCKET connection, Packet data) {
			int packet_int = (int)data;
			if (!sendInt(connection, packet_int))
				return false;
			return true;
		}

		bool recvPacket(SOCKET connection, Packet& data) {
			int packet_int = 0;
			if (!recvInt(connection, packet_int)) {
				return false;
			}
			Packet packet = (Packet)packet_int;
			data = packet;
			return true;
		}
	}

	class Server {
		static Server* instance;

		std::string server_ip;
		unsigned int server_port;

		SOCKADDR_IN addr;
		int addrlen = sizeof(addr);

		bool (*handlePacket)(SOCKET connection, Packet packet);

		static void thread_handler(SOCKET connection) {
			Packet data;
			while (true) {
				if (!Net::recvPacket(connection, data))
					break;
				if (!instance->handlePacket(connection, data))
					break;
			}
			closesocket(connection);
		}

		static void thread_listenConnections() {
			while (true) {
				SOCKET connection = accept(instance->listenSocket, (sockaddr*)& instance->addr, &instance->addrlen);
				if (connection) {
					CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_handler, (LPVOID)connection, 0, 0);
				}
			}
		}

		public:
		SOCKET listenSocket;

		Server(std::string server_ip, unsigned int server_port, bool (*handlePacket)(SOCKET connection, Packet packet)) {
			this->server_ip = server_ip;
			this->server_port = server_port;
			this->handlePacket = handlePacket;

			instance = this;
		}

		bool start() {
			WSADATA data = { 0 };
			WORD version = MAKEWORD(1, 1);

			if (WSAStartup(version, &data))
				return false;

			addr.sin_addr.s_addr = inet_addr(this->server_ip.c_str());
			addr.sin_port = htons(this->server_port);
			addr.sin_family = AF_INET;

			this->listenSocket = socket(AF_INET, SOCK_STREAM, NULL);

			if (this->listenSocket == INVALID_SOCKET)
				return false;
			if (bind(this->listenSocket, (sockaddr*)& addr, addrlen) == SOCKET_ERROR)
				return false;
			if (listen(this->listenSocket, SOMAXCONN) == SOCKET_ERROR)
				return false;

			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_listenConnections, 0, 0, 0);

			return true;
		}

		void stop() {
			closesocket(this->listenSocket);
			WSACleanup();
		}
	};
	Server* Server::instance;

	class Client {
		static Client* instance;

		std::string server_ip;
		unsigned int server_port;

		SOCKADDR_IN addr;
		int addrlen = sizeof(addr);

		bool (*handlePacket)(SOCKET connection, Packet packet);

		static void thread_clientHandler(SOCKET connection) {
			Packet data;
			while (true) {
				if (!Net::recvPacket(connection, data))
					break;
				if (!instance->handlePacket(connection, data))
					break;
				Sleep(300);
			}
			instance->stop();
		}

		public:
		SOCKET serverSocket;

		Client(std::string server_ip, unsigned int server_port, bool (*handlePacket)(SOCKET connection, Packet packet)) {
			this->server_ip = server_ip;
			this->server_port = server_port;
			this->handlePacket = handlePacket;

			instance = this;
		}

		bool start() {
			WSADATA data = { 0 };
			WORD version = MAKEWORD(1, 1);

			if (WSAStartup(version, &data))
				return false;

			addr.sin_addr.s_addr = inet_addr(this->server_ip.c_str());
			addr.sin_port = htons(this->server_port);
			addr.sin_family = AF_INET;

			this->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (connect(this->serverSocket, (sockaddr*)& this->addr, this->addrlen) != NULL)
				return false;

			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_clientHandler, (LPVOID)this->serverSocket, 0, 0);

			return true;
		}

		void stop() {
			closesocket(this->serverSocket);
			WSACleanup();
		}
	};
	Client* Client::instance;
}
