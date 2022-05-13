#include "ns_utils.h"

int verify_domain_addr(char *addr) {
    int i;
    char c;
    int len = strlen(addr);

    // Check the domain address length
    if (len < 2) {
        printf("\t\t > ERROR - `%s`: Domain address is too short\n", addr);
        return 1;
    }
    if (len > 63) {
        printf("\t\t > ERROR - `%s`: Domain address is too long\n", addr);
        return 1;
    }

    // Check all characters
    for (i = 0; i < len; i++) {
        c = addr[i];
        if (((c > 'a') && (c < 'z')) || ((c > '0') && (c < '9')) || (c == '-') || (c == '.')) {
            if (((i - 3) >= 0) && ((i + 1) < len)) {
                if ((c == '-') && (addr[i - 3] == '.') && (addr[i + 1] == '-')) {
                    printf("\t\t > ERROR - `%s`: The domain address does't meet domain address rouls\n", addr);
                    return 1;
                }
            }
        }
    }
    return 0;
}

int ScanHostName(char *host) {
    printf("\n\t > Please Enter Host Name:\n\t\t > ");
    if (scanf("%s", host) == 0) {
        printf("\n\t\tERROR: Failed Scanning Host Name");
        return 1;
    }
    return 0;
}

void SetDnsRequest(DNS_HEADER *dns) {
    dns->id = (unsigned short)(htons(GetCurrentProcessId()));
    dns->qr = 0;      // This is a query
    dns->opcode = 0;  // This is a standard query
    dns->aa = 0;      // Not Authoritative
    dns->tc = 0;      // This message is not truncated
    dns->rd = 1;      // Recursion Desired
    dns->ra = 0;      // Recursion not available! hey we dont have it (lol)
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1);  // we have only 1 question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;
}

void SendDnsQuery(SOCKET s, char *buf, char *name, SOCKADDR_IN dest, QUESTION *info) {
    int i = sizeof(dest);
    info = (struct QUESTION *)&buf[sizeof(struct DNS_HEADER) + (strlen((const char *)name) + 1)];  // fill it
    info->type = htons(1);                                                                         // ipv4 address
    info->class = htons(1);                                                                        // its internet
    // printf("\t\t\t -> Sending Packet\n");
    if (sendto(s, (char *)buf, sizeof(struct DNS_HEADER) + (strlen((const char *)name) + 1) + sizeof(struct QUESTION), 0, (struct sockaddr *)&dest, sizeof(dest)) == SOCKET_ERROR) {
        printf("\n\t\t\tERROR: Failed Sending Query (%d)", WSAGetLastError());
    }
}

void GetAnswer(SOCKET s, char *buf, SOCKADDR_IN dest, char *name, char *host_name) {
    int i = sizeof(dest);
    int stop;
    char *reader;
    SOCKADDR_IN a;
    RES_RECORD answer;
    // printf("\t\t\t <- Receiving Answer\n");
    if (recvfrom(s, (char *)buf, 65536, 0, (struct sockaddr *)&dest, &i) == SOCKET_ERROR) {
        printf("\n\t\t\tERROR: Failed Getting Answer (%d)", WSAGetLastError());
    }

    // reading answers
    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char *)name) + 1) + sizeof(struct QUESTION)];
    stop = 0;
    answer.name = ReadName(reader, buf, &stop);
    reader = reader + stop;
    answer.resource = (struct R_DATA *)(reader);
    reader = reader + sizeof(struct R_DATA);

    host_name[strlen(host_name) - 1] = '\0';
    if (ntohs(answer.resource->type) == 1) {
        // Has IPv4 Address
        answer.rdata = (unsigned char *)malloc(ntohs(answer.resource->data_len));
        for (int j = 0; j < ntohs(answer.resource->data_len); j++) {
            answer.rdata[j] = reader[j];
        }
        answer.rdata[ntohs(answer.resource->data_len)] = '\0';
        reader = reader + ntohs(answer.resource->data_len);
        long *p = (long *)answer.rdata;
        a.sin_addr.s_addr = (*p);
        printf("\t\t > IPv4 = %s\n", inet_ntoa(a.sin_addr));
    } else {
        answer.rdata = ReadName(reader, buf, &stop);
        reader = reader + stop;
        printf("\t\t > Could not reach `%s`\n", host_name);
    }
}

void GetHost(unsigned char *host, char *ip) {
    unsigned char buf[65536], *name;
    SOCKET s;
    SOCKADDR_IN dest;

    DNS_HEADER *dns = NULL;
    QUESTION *info = NULL;

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // UDP packet for DNS queries

    // Configure the sockaddress structure with information of DNS server
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(ip);

    // Set the DNS structure to standard queries
    dns = (struct DNS_HEADER *)&buf;

    SetDnsRequest(dns);
    // point to the query portion
    name = (unsigned char *)&buf[sizeof(struct DNS_HEADER)];
    DnsFormat(name, host);
    SendDnsQuery(s, buf, name, dest, info);
    GetAnswer(s, buf, dest, name, host);
    dns = (struct DNS_HEADER *)buf;
    // move ahead of the dns header and the query field

    return;
}

unsigned char *ReadName(unsigned char *reader, unsigned char *buffer, int *count) {
    unsigned char *name;
    unsigned int p = 0, jumped = 0, offset;
    int i, j;

    *count = 1;
    name = (unsigned char *)malloc(256);

    name[0] = '\0';

    // read the names in 3www6google3com format
    while (*reader != 0) {
        if (*reader >= 192) {
            offset = (*reader) * 256 + *(reader + 1) - 49152;  // 49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1;  // we have jumped to another location so counting wont go up!
        } else {
            name[p++] = *reader;
        }

        reader = reader + 1;

        if (jumped == 0) *count = *count + 1;  // if we havent jumped to another location then we can count up
    }

    name[p] = '\0';  // string complete
    if (jumped == 1) {
        *count = *count + 1;  // number of steps we actually moved forward in the packet
    }

    // now convert 3www6google3com0 to www.google.com
    for (i = 0; i < (int)strlen((const char *)name); i++) {
        p = name[i];
        for (j = 0; j < (int)p; j++) {
            name[i] = name[i + 1];
            i = i + 1;
        }
        name[i] = '.';
    }

    name[i - 1] = '\0';  // remove the last dot

    return name;
}

void DnsFormat(unsigned char *dns, unsigned char *host) {
    int lock = 0, i;

    strcat((char *)host, ".");

    for (i = 0; i < (int)strlen((char *)host); i++) {
        if (host[i] == '.') {
            *dns++ = i - lock;
            for (; lock < i; lock++) {
                *dns++ = host[lock];
            }
            lock++;  // or lock=i+1;
        }
    }
    *dns++ = '\0';
}