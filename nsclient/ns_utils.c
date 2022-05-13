#include "ns_utils.h"

// List of DNS Servers registered on the system
char dns_servers[10][100];

void SetDnsRequest(DNS_HEADER *dns) {
    dns->id = (unsigned short)htons(GetCurrentProcessId());
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
    info = (struct QUESTION *)&buf[sizeof(struct DNS_HEADER) + (strlen((const char *)name) + 1)];  // fill it
    info->type = htons(1);                                                                         // ipv4 address
    info->class = htons(1);                                                                        // its internet

    int i = sizeof(dest);
    printf("\n\t\t -> Sending Packet");
    if (sendto(s, (char *)buf, sizeof(struct DNS_HEADER) + (strlen((const char *)name) + 1) + sizeof(struct QUESTION), 0, (struct sockaddr *)&dest, sizeof(dest)) == SOCKET_ERROR) {
        printf("\n\t\tERROR: Failed Sending Query (%d)", WSAGetLastError());
    }
}

void GetHost(unsigned char *host, char *ip) {
    unsigned char buf[65536], *name, *reader;
    int i, j, stop;

    SOCKET s;
    SOCKADDR_IN dest, a;
    RES_RECORD answers[20], auth[20], addit[20];  // the replies from the DNS server

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

    i = sizeof(dest);
    printf("\nReceiving answer...");
    if (recvfrom(s, (char *)buf, 65536, 0, (struct sockaddr *)&dest, &i) == SOCKET_ERROR) {
        printf("Failed. Error Code : %d", WSAGetLastError());
    }
    printf("Received.");

    dns = (struct DNS_HEADER *)buf;
    // move ahead of the dns header and the query field
    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char *)name) + 1) + sizeof(struct QUESTION)];

    // reading answers
    stop = 0;

    for (i = 0; i < ntohs(dns->ans_count); i++) {
        answers[i].name = ReadName(reader, buf, &stop);
        reader = reader + stop;

        answers[i].resource = (struct R_DATA *)(reader);
        reader = reader + sizeof(struct R_DATA);

        if (ntohs(answers[i].resource->type) == 1)  // if its an ipv4 address
        {
            answers[i].rdata = (unsigned char *)malloc(ntohs(answers[i].resource->data_len));

            for (j = 0; j < ntohs(answers[i].resource->data_len); j++)
                answers[i].rdata[j] = reader[j];

            answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';

            reader = reader + ntohs(answers[i].resource->data_len);

        } else {
            answers[i].rdata = ReadName(reader, buf, &stop);
            reader = reader + stop;
        }
    }

    // read authorities
    for (i = 0; i < ntohs(dns->auth_count); i++) {
        auth[i].name = ReadName(reader, buf, &stop);
        reader += stop;

        auth[i].resource = (struct R_DATA *)(reader);
        reader += sizeof(struct R_DATA);

        auth[i].rdata = ReadName(reader, buf, &stop);
        reader += stop;
    }

    // read additional
    for (i = 0; i < ntohs(dns->add_count); i++) {
        addit[i].name = ReadName(reader, buf, &stop);
        reader += stop;

        addit[i].resource = (struct R_DATA *)(reader);
        reader += sizeof(struct R_DATA);

        if (ntohs(addit[i].resource->type) == 1) {
            addit[i].rdata = (unsigned char *)malloc(ntohs(addit[i].resource->data_len));
            for (j = 0; j < ntohs(addit[i].resource->data_len); j++)
                addit[i].rdata[j] = reader[j];

            addit[i].rdata[ntohs(addit[i].resource->data_len)] = '\0';
            reader += ntohs(addit[i].resource->data_len);

        } else {
            addit[i].rdata = ReadName(reader, buf, &stop);
            reader += stop;
        }
    }

    // print answers
    for (i = 0; i < ntohs(dns->ans_count); i++) {
        // printf("\nAnswer : %d",i+1);
        printf("Name : %s ", answers[i].name);

        if (ntohs(answers[i].resource->type) == 1)  // IPv4 address
        {
            long *p;
            p = (long *)answers[i].rdata;
            a.sin_addr.s_addr = (*p);  // working without ntohl
            printf("has IPv4 address : %s", inet_ntoa(a.sin_addr));
        }
        if (ntohs(answers[i].resource->type) == 5)  // Canonical name for an alias
        {
            printf("has alias name : %s", answers[i].rdata);
        }

        printf("\n");
    }

    // print authorities
    for (i = 0; i < ntohs(dns->auth_count); i++) {
        // printf("\nAuthorities : %d",i+1);
        printf("Name : %s ", auth[i].name);
        if (ntohs(auth[i].resource->type) == 2) {
            printf("has authoritative nameserver : %s", auth[i].rdata);
        }
        printf("\n");
    }

    // print additional resource records
    for (i = 0; i < ntohs(dns->add_count); i++) {
        // printf("\nAdditional : %d",i+1);
        printf("Name : %s ", addit[i].name);
        if (ntohs(addit[i].resource->type) == 1) {
            long *p;
            p = (long *)addit[i].rdata;
            a.sin_addr.s_addr = (*p);  // working without ntohl
            printf("has IPv4 address : %s", inet_ntoa(a.sin_addr));
        }
        printf("\n");
    }

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

// this will convert www.google.com to 3www6google3com ;got it :)
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