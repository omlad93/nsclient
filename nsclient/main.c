#include "ns_utils.h"

#define MAX_LENGTH 2500

int verify_domain_addr(char* addr) {
    int i;
    char c;
    int len = strlen(addr);
    // Check the domain address length
    if (len < 2) {
        printf("Domain address is too short\n");
        return 1;
    }
    if (len > 63) {
        printf("Domain address is too long\n");
        return 1;
    }

    // Check all characters
    for (i = 0; i < len; i++) {
        c = addr[i];
        if (((c > 'a') && (c < 'z')) || ((c > '0') && (c < '9')) || (c == '-') || (c == '.')) {
            if (((i - 3) >= 0) && ((i + 1) < len)) {
                if ((c == '-') && (addr[i - 3] == '.') && (addr[i + 1] == '-')) {
                    printf("The domain address does't meet domain address rouls\n");
                    return 1;
                }
            }
        }
    }
    return 0;
}

int ScanHostName(char* host) {
    printf("\nPlease Enter Host Name:\n\t > ");
    if (scanf("%s", host) == 0) {
        printf("\n\tERROR: Failed Scanning Host Name\n");
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[])  // do you know what is int main() ?
{
    unsigned char hostname[100];
    char domain[MAX_LENGTH];
    int status;
    char* ip = argv[1];
    WSADATA firstsock;

    // RetrieveDnsServersFromRegistry();

    printf("\nRunning nsclient for: %s", ip);
    if (WSAStartup(MAKEWORD(2, 2), &firstsock) != 0) {
        printf("\n\tERROR: Failed Initialising Winsock (%d)", WSAGetLastError());
        return 1;
    }

    while (ScanHostName(domain)) {
        // Runs Until Host Name Accepted
    }

    while (strcmp(domain, "quit") != 0) {
        // syntactic verification of the domain address
        status = verify_domain_addr(domain);
        if (status) {
            printf("\n\t\tERROR: Received Bad Name");
        } else {
            GetHost(domain, ip);
        }
        ScanHostName(domain);
    }

    return 0;
}