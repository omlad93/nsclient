#include "ns_utils.h"

#define MAX_LENGTH 2500

int verify_domain_addr(char* addr) {
    int i;
    char c;
    int len = strlen(addr);
    
    // Check the domain address length
    if (len < 2) {
        printf("\t\t > ERROR - `%s`: Domain address is too short\n",addr);
        return 1;
    }
    if (len > 63) {
        printf("\t\t > ERROR - `%s`: Domain address is too long\n",addr);
        return 1;
    }

    // Check all characters
    for (i = 0; i < len; i++) {
        c = addr[i];
        if (((c > 'a') && (c < 'z')) || ((c > '0') && (c < '9')) || (c == '-') || (c == '.')) {
            if (((i - 3) >= 0) && ((i + 1) < len)) {
                if ((c == '-') && (addr[i - 3] == '.') && (addr[i + 1] == '-')) {
                    printf("\t\t > ERROR - `%s`: The domain address does't meet domain address rouls\n",addr);
                    return 1;
                }
            }
        }
    }
    return 0;
}

int ScanHostName(char* host) {
    printf("\n\t > Please Enter Host Name:\n\t\t > ");
    if (scanf("%s", host) == 0) {
        printf("\n\t\tERROR: Failed Scanning Host Name");
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[])  // do you know what is int main() ?
{
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
            // Error Printed in verify_domain_addr()
        } else {
            GetHost(domain, ip);
        }
        ScanHostName(domain);
    }

    return 0;
}