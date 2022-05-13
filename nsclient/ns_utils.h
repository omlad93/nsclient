#pragma once
#include "conio.h"
#include "stdio.h"
#include "structs.h"
#include "windows.h"
//#include "winsock2.h"
//#pragma comment(lib, "Ws2_32.lib")

// function for setting the Header
void SetDnsRequest(DNS_HEADER *dns);

// function for sending query request to the server
void SendDnsQuery(SOCKET s, char *buf, char *name, SOCKADDR_IN dest, QUESTION *info);

// function for getting & parsing the answer from server
void GetAnswer(SOCKET s, char *buf, SOCKADDR_IN dest, char *name, char *host_name);

// Function for querying ip for host
void GetHost(unsigned char *host, char *ip);

// Helper function for parsing answers
unsigned char *ReadName(unsigned char *reader, unsigned char *buffer, int *count);

// Converting DNS format: from google.com to 6google3com )
void DnsFormat(unsigned char *dns, unsigned char *host);