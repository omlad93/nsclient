# NSCLIENT

## Overview
The `nsclient` program is implementation of a simple DNS client.  
The program gets a single command line argument which is the IP address of a DNS server, given
in dotted-decimal notation (`8.8.8.8` for example).  
Given a DNS server, the program asks the user to enter the requested host (domain) and if it seems that is is possible host name, it sends the server a query the requested domain.  
the programs keeps asking for more domain, until the user type `quit`.  
The verification of the Domain is based on the rules from <a href="https://www.dnsbelgium.be/en/register-domain-name/valid" target="_blank">dns-belgium</a>

**`Iris Taubkin`** 208410969  
**`Omri Elad`**    204620702  


## Implementation
The program is divided into 3 modules:
| idx | name     | files                  | comments                                    |
| --- | -------- | ---------------------- | ------------------------------------------- |
| 1   | structs  | structs.h              | all the data structures needed for nsclient |
| 2   | ns_utils | ns_utils.c, ns_utils.h | all the functions needed in main            |
| 3   | main     | main.c                 | Main function: loop over domains            |

### **STRUCTS Module**
This Module is holding the necessary C-structures for querying a DNS server, matching for the message format represented in the assignment's appendix:

```                                                        
+---------------------+  
| Header              |  
+---------------------+  
| Question            |  the question for the name server  
+---------------------+  
| Answer              |  RRs answering the question  
+---------------------+  
| Authority           |  RRs pointing toward an authority  
+---------------------+  
| Additional          |  RRs holding additional information  
+---------------------+  
```
We have the following structs, with the fields for the the information that changes from one message to another:
```C
typedef struct DNS_HEADER   DNS_HEADER;
typedef struct QUESTION     QUESTION;
typedef struct R_DATA       R_DATA;
typedef struct RES_RECORD   RES_RECORD;
```
along with some macro definitions.  


### **NS_UTILS Module**
This is module containing all the functionality of querying a DNS server for domains.
it provides a function for a single query:
```C
#define TIMEOUT 3000

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
```


### **Main Module**  
The Main module is providing the required interface of the assignment:
it has a single function, which is compiled into the `.exe` file:
```C
 int main(int argc, char* argv[]);
``` 
this function expects a single argument: the address of a SND Server in a dotted-decimal format: such as `8.8.8.8` (Google\`s DNS server)  
this function is an implementation of loop that asks the user for a domain to look for in each iteration, until it receives '`quit`'.  
in each iteration the function runs a soft verification for the syntax of the domain, and if it seems as a legitimate domain a query request is sent using:
```C
GetHost(domain, ip);
```
<br>  

## Connection Timeout
During debugging, when a wrong Server ip was given as input, we encountered failures on sending / receiving packets that led to forced termination of the program.  
In order to deal with such cases we added timeout for `sendto()` and `recvfrom()` using `setsockopt()` function from `winsock2` library. The timeout length (in miliseconds is defined in `ns_utils.h` as `TIMEOUT` macro definition)

## Initialization Process on Visual Studio
All this should be already configured in our solution. In case of an error follow this section:  
In order to be able to build `nsclient` properly, make sure to follow this steps:
 * add `Ws2_32.lib` to `Linker->Input->Additional Dependencies`
 * add `_CRT_SECURE_NO_WARNINGS;_WINSOCK_DEPRECATED_NO_WARNINGS;` to `C/C++->Preprocessor->Preprocessor Definitions` to avoid VS C limitations.
 * ignore unsafe functions: add `4996` to `Properties > C/C++ > Advanced property->Disable Specific Warnings`


## Running Output Example
```Console
Running nsclient for: 8.8.8.8

         > Please Enter Host Name:
                 > google.com
                 > IPv4 = 142.250.185.174

         > Please Enter Host Name:
                 > n12.co.il
                 > IPv4 = 81.218.110.166

         > Please Enter Host Name:
                 > bakara.eng.tau.il
                 > Could not reach `bakara.eng.tau.il`

         > Please Enter Host Name:
                 > bakara.eng.tau.ac.il
                 > IPv4 = 132.66.48.12

         > Please Enter Host Name:
                 > Omri.Elad.and.Iris.Taubkin
                 > Could not reach `Omri.Elad.and.Iris.Taubkin`

         > Please Enter Host Name:
                 > .
                 > ERROR - `.`: Domain address is too short

         > Please Enter Host Name:
                 > Now.Lets.Try.A.Long.Domain.To.Look.For.And.See.What.Happends:
                 > Could not reach `Now.Lets.Try.A.Long.Domain.To.Look.For.And.See.What.Happends:`

         > Please Enter Host Name:
                 > Now.Lets.Try.A.Really.Long.Domain.To.Look.For.And.See.What.Will.Happend.Now:
                 > ERROR - `Now.Lets.Try.A.Really.Long.Domain.To.Look.For.And.See.What.Will.Happend.Now:`: Domain address is too long

         > Please Enter Host Name:
                 > quit
```
