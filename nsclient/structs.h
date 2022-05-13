
// Type field of Query and Answer
#define T_A 1     /* host address */
#define T_NS 2    /* authoritative server */
#define T_CNAME 5 /* canonical name */
#define T_SOA 6   /* start of authority zone */
#define T_PTR 12  /* domain name pointer */
#define T_MX 15   /* mail routing information */

// DNS header structure
typedef struct DNS_HEADER {
    unsigned short id;  // identification number

    unsigned char rd : 1;      // recursion desired
    unsigned char tc : 1;      // truncated message
    unsigned char aa : 1;      // authoritive answer
    unsigned char opcode : 4;  // purpose of message
    unsigned char qr : 1;      // query/response flag

    unsigned char rcode : 4;  // response code
    unsigned char cd : 1;     // checking disabled
    unsigned char ad : 1;     // authenticated data
    unsigned char z : 1;      // its z! reserved
    unsigned char ra : 1;     // recursion available

    unsigned short q_count;     // number of question entries
    unsigned short ans_count;   // number of answer entries
    unsigned short auth_count;  // number of authority entries
    unsigned short add_count;   // number of resource entries
} DNS_HEADER;

// Constant sized fields of query structure
typedef struct QUESTION {
    unsigned short type;
    unsigned short class;
} QUESTION;

// Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct R_DATA {
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)
typedef struct R_DATA R_DATA;
// Pointers to resource record contents
typedef struct RES_RECORD {
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
} RES_RECORD;


