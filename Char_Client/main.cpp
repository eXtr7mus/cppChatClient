#include <Windows.h>
#include "resource.h"
#include <string>
#include "ClientSocket.h"
#include <sstream>
#pragma comment(lib, "ws2_32.lib")


const char g_czClassName[] = "myClass";
std::string ipaddr, port, name;
std::string GetTextEditMsg(HWND hwnd, int idcHandle);
DWORD WINAPI thread2(LPVOID t);

BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM) {
	switch (Message) {
		case WM_INITDIALOG:
			SetDlgItemText(hwnd, IDC_TEXT, "127.0.0.1");
			SetDlgItemText(hwnd, IDC_TEXT2, "54000");
			SetDlgItemText(hwnd, IDC_TEXT3, "Enter your nickname");
		break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					ipaddr = GetTextEditMsg(hwnd, IDC_TEXT);
					port = GetTextEditMsg(hwnd, IDC_TEXT2);
					name = GetTextEditMsg(hwnd, IDC_TEXT3);
					EndDialog(hwnd, IDOK);
				}
				break;
				case IDCANCEL:
					EndDialog(hwnd, IDCANCEL);
				break;
			}
		break;
		default:
			return FALSE;
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
	std::string msg = "";

	ClientSocket *sock;

	switch (Message) {
		case WM_CREATE: {

			LPCREATESTRUCT lpCreateStruct = (LPCREATESTRUCT)lParam;
			sock = (ClientSocket*)lpCreateStruct->lpCreateParams;
			SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG)sock);
			

			HFONT hfDefault;
			HWND hEditMsgEnter;
			HWND hEditMsgShow;

			hEditMsgShow = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY,
				0, 0, 400, 400,
				hwnd, (HMENU)IDC_MAIN_MESSAGES, GetModuleHandle(NULL), NULL);

			hEditMsgEnter = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
				0, 0, 100, 100,
				hwnd, (HMENU)IDC_MAIN_ENTER, GetModuleHandle(NULL), NULL);

			if (hEditMsgEnter == NULL || hEditMsgShow == NULL) {
				MessageBox(hwnd, "Cant create edit box", "Err", MB_OK);
			}
			hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hEditMsgEnter, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			SendMessage(hEditMsgShow, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		}
		break;
		case WM_SIZE: {
			HWND hEdit;
			RECT rcClient;
			
			GetClientRect(hwnd, &rcClient);

			hEdit = GetDlgItem(hwnd, IDC_MAIN_ENTER);
			SetWindowPos(hEdit, NULL, 0, 300, rcClient.right-80, rcClient.bottom, SWP_NOZORDER);
			hEdit = GetDlgItem(hwnd, IDC_MAIN_MESSAGES);
			SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, 300, SWP_NOZORDER);
		}
		break;
		case WM_KEYDOWN:
			switch (LOWORD(wParam)) {
				case VK_RETURN: {
					msg = "ME : " + msg + GetTextEditMsg(hwnd, IDC_MAIN_ENTER) + "\r\n";
					sock = (ClientSocket*)GetWindowLongPtr(hwnd, GWL_USERDATA);
					sock->Send(GetTextEditMsg(hwnd, IDC_MAIN_ENTER));
					std::string copy(msg);
					const char *message = copy.c_str();
					HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_MESSAGES);
					int ndx = GetWindowTextLength(hEdit);
					SetFocus(hEdit);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)((LPSTR)message));
					SetDlgItemText(hwnd, IDC_MAIN_ENTER, "");
					SetFocus(GetDlgItem(hwnd, IDC_MAIN_ENTER));
				}
				break;
			}
		break;
		case WM_ENABLE: {
			char buff[4069];
			ZeroMemory(&buff, 4069);
			sock = (ClientSocket*)GetWindowLongPtr(hwnd, GWL_USERDATA);
			int bytesIn = recv(sock->getSocket(), buff, 4069, 0);
			HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_MESSAGES);
			int ndx = GetWindowTextLength(hEdit);
			SetFocus(hEdit);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)((LPSTR)buff));
			SetFocus(GetDlgItem(hwnd, IDC_MAIN_ENTER));

		}
		break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_FILE_EXIT: {
					SendMessage(hwnd, WM_CLOSE, 0, 0);
				}
			break;
			}
		break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	HWND hWnd;
	MSG msg;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = g_czClassName;
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		MessageBox(0, "Window Reg Failed", "Err", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ClientSocket client;

	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, g_czClassName,
		"Chat client", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		600, 400, NULL, NULL, hInstance, (LPVOID)&client);
	if (hWnd == NULL) {
		MessageBox(0, "Window Creation Failed", "Err", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc);
	if (ret == IDOK) {
		//	MessageBox(hwnd, "Ok", "ok", MB_OK);
	}
	else if (ret == -1) {
		MessageBox(hWnd, "DIALOG FAILED", "DIALOG FAILED", MB_OK);
	}
	
	client.FillIn(ipaddr, atoi(port.c_str()), name);

	if (client.Init()) {
		client.Run();
	}
	
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		if (msg.message == WM_KEYDOWN) SendMessage(hWnd, WM_KEYDOWN, msg.wParam, msg.lParam);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}


std::string GetTextEditMsg(HWND hwnd, int idcHandle) {
	int len = GetWindowTextLength(GetDlgItem(hwnd, idcHandle));
	if (len > 0) {
		char *buf;
		buf = (char*)GlobalAlloc(GPTR, len + 1);
		GetDlgItemText(hwnd, idcHandle, buf, len + 1);
		std::string result(buf);
		GlobalFree((HANDLE)buf);
		return result;
	}
	return nullptr;
}

DWORD thread2(LPVOID t)
{
	LPCREATESTRUCT lpCreateStruct = (LPCREATESTRUCT)t;
	ClientSocket *sock = (ClientSocket*)lpCreateStruct->lpCreateParams;
	fd_set master;
	FD_ZERO(&master);
	FD_SET(sock->getSocket(), &master);
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 3000;
	while (true) {
		fd_set copy = master;
		int socketCount = select(0, &copy, nullptr, nullptr, &tv);
		if (socketCount > 0) {
			SendMessage(hWnd, WM_ENABLE, NULL, NULL);
		}
		return 0;
}
