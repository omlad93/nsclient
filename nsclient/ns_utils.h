#pragma once
//Header Files
#include "winsock2.h"
#include "windows.h"
#include "stdio.h"
#include "conio.h"
#include "structs.h"

// #pragma comment(lib,"ws2_32.lib") //Winsock Library

//Function Declarations
void ngethostbyname (unsigned char*);
void ChangetoDnsNameFormat (unsigned char*,unsigned char*);
unsigned char* ReadName (unsigned char*,unsigned char*,int*);
void RetrieveDnsServersFromRegistry(void);
unsigned char* PrepareDnsQueryPacket (unsigned char*);
