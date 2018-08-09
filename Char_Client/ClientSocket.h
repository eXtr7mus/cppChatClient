#pragma once
#include <string>
#include <Windows.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

class ClientSocket
{
private:
	std::string m_ipaddr;
	int m_port;
	std::string m_name;
	SOCKET m_sock;
	HWND hwnd;
public:
	ClientSocket( std::string ipaddr, int port, std::string name);
	ClientSocket();
	~ClientSocket();
	void FillIn(std::string ipaddr, int port, std::string name);
	void setHwnd(HWND hwnd);
	bool Init();
	void Run();
	void Send(std::string msg);
	SOCKET CreateSocket();
	void Cleanup();
	SOCKET getSocket();
};

