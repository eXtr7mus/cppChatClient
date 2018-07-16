#include "ClientSocket.h"


ClientSocket::ClientSocket(HWND hwnd, std::string ipaddr, int port, std::string name):
hwnd(hwnd),m_ipaddr(ipaddr), m_port(port), m_name(name)
{
}

ClientSocket::~ClientSocket()
{
	Cleanup();
}

bool ClientSocket::Init()
{
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		MessageBox(hwnd, "Cant initialize winsock", "Error", MB_OK);
		return false;
	}
	return true;
}

void ClientSocket::Run()
{
	SOCKET clientSock = CreateSocket();

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(m_port);
	//inet_pton(AF_INET, m_ipaddr.c_str(), &hint.sin_addr);
	hint.sin_addr.s_addr = inet_addr(m_ipaddr.c_str());
	
	int connResult = connect(clientSock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		MessageBox(hwnd, "Cant connect to server", "Error", MB_OK);
		system("pause");
		closesocket(clientSock);
		WSACleanup();
		return;
	}
	Send(clientSock, m_name);
	char buff[4096];
		ZeroMemory(buff, 4096);
	int bytesReceived = recv(clientSock, buff, 4096, 0);
	if (bytesReceived > 0)
	{
		MessageBox(hwnd, buff, "Hello from server", MB_OK);
	}
}

void ClientSocket::Send(int clientSocket, std::string msg) {
	send(clientSocket, msg.c_str(), msg.size() + 1, 0);
}

SOCKET ClientSocket::CreateSocket()
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		MessageBox(hwnd, "Cant create socket", "Error", MB_OK);
		WSACleanup();
	}
	return sock;
}

void ClientSocket::Cleanup()
{
	WSACleanup();
}
