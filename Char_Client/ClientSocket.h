#pragma once
#include <string>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")

class ClientSocket
{
private:
	std::string m_ipaddr;
	int m_port;
	std::string m_name;
	HWND hwnd;
public:
	ClientSocket(HWND hwnd, std::string ipaddr, int port, std::string name);
	~ClientSocket();
	bool Init();
	void Run();
	void Send(int clientSocket, std::string msg);
	SOCKET CreateSocket();
	void Cleanup();
};

