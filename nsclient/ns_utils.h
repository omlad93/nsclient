#pragma once
#include "conio.h"
#include "stdio.h"
#include "structs.h"
#include "windows.h"
//#include "winsock2.h"
//#pragma comment(lib, "Ws2_32.lib")


// Function Declarations
void GetHost(unsigned char* host, char* ip);

unsigned char* ReadName(unsigned char*, unsigned char*, int*);

void RetrieveDnsServersFromRegistry(void);
unsigned char* PrepareDnsQueryPacket(unsigned char*);

void DnsFormat(unsigned char* dns, unsigned char* host);