#include "ns_utils.h"

int main() //do you know what is int main() ?
{
	unsigned char hostname[100];

	//RetrieveDnsServersFromRegistry();
	
	WSADATA firstsock;
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&firstsock) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}
	printf("Initialised.");

	printf("\nEnter Hostname to Lookup : ");
	gets((char*)hostname);
	ngethostbyname(hostname);

	_getch();
	return 0;
}