#include "ClientSocket.h"


ClientSocket::ClientSocket(std::string ipaddr, int port, std::string name):
m_ipaddr(ipaddr), m_port(port), m_name(name)
{
}

ClientSocket::ClientSocket() {

}

ClientSocket::~ClientSocket()
{
	Cleanup();
}

void ClientSocket::FillIn(std::string ipaddr, int port, std::string name)
{
	m_ipaddr = ipaddr;
	m_port = port;
	m_name = name;
}

void ClientSocket::setHwnd(HWND hwnd){
	this->hwnd = hwnd;
}

bool ClientSocket::Init()
{
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		//MessageBox(hwnd, "Cant initialize winsock", "Error", MB_OK);
		return false;
	}
	return true;
}

void ClientSocket::Run()
{
	m_sock = CreateSocket();

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(m_port);
	hint.sin_addr.s_addr = inet_addr(m_ipaddr.c_str());
	
	int connResult = connect(m_sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		//MessageBox(hwnd, "Cant connect to server", "Error", MB_OK);
		system("pause");
		closesocket(m_sock);
		WSACleanup();
		return;
	}
	Send(m_name);
	char buff[4096];
		ZeroMemory(buff, 4096);
	int bytesReceived = recv(m_sock, buff, 4096, 0);
	std::string temp(buff);
	if (bytesReceived > 0)
	{
		//MessageBox(hwnd, buff, "Hello from server", MB_OK);
	}
	if (temp.find("/wn") != std::string::npos) {
		MessageBox(hwnd, "this nickname is already in use, restart the client", "this nickname is already in use!", MB_OK);
	}
}

void ClientSocket::Send(std::string msg) {
	send(m_sock, msg.c_str(), msg.size() + 1, 0);
}

SOCKET ClientSocket::CreateSocket()
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		//MessageBox(hwnd, "Cant create socket", "Error", MB_OK);
		WSACleanup();
	}
	return sock;
}

void ClientSocket::Cleanup()
{
	WSACleanup();
}

SOCKET ClientSocket::getSocket()
{
	return m_sock;
}
