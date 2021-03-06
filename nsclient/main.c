#include "ns_utils.h"

#define MAX_LENGTH 2500

int main(int argc, char* argv[]) {
    char domain[MAX_LENGTH];
    char* ip = argv[1];
    WSADATA firstsock;
    printf("Running nsclient for: %s\n", ip);
    if (WSAStartup(MAKEWORD(2, 2), &firstsock) != 0) {
        printf("\n\tERROR: Failed Initialising Winsock (%d)", WSAGetLastError());
        return 1;
    }

    ScanHostName(domain);
    while (strcmp(domain, "quit") != 0) {
        // syntactic verification of the domain address
        if (verify_domain_addr(domain)) {
            GetHost(domain, ip, TIMEOUT);
        }
        ScanHostName(domain);
    }

    return 0;
}