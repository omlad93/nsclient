#pragma once
#include "conio.h"
#include "stdio.h"
#include "structs.h"
#include "windows.h"


#define TIMEOUT 3000
#define MAX_DOMAIN_SZ 100

// Check format of host for lookup: retrun 1 for good, 0 for bad
int verify_domain_addr(char *addr);

// Scanning required host to look for
int ScanHostName(char *host);

// function for setting the Header
void SetDnsRequest(DNS_HEADER *dns);

// function for sending query request to the server
int SendDnsQuery(SOCKET s, char *buf, char *name, SOCKADDR_IN dest, QUESTION *info, int timeout);

// function for getting & parsing the answer from server
int GetAnswer(SOCKET s, char *buf, SOCKADDR_IN dest, char *name, char *host_name, int timeout);

// Function for querying ip for host
void GetHost(unsigned char *host, char *ip, int timeout);

// Helper function for parsing answers
unsigned char *ReadName(unsigned char *reader, unsigned char *buffer, int *count);

// Converting DNS format: from google.com to 6google3com )
void DnsFormat(unsigned char *dns, unsigned char *host);