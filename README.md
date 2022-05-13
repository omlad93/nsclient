# NSCLIENT

## Overview
The `nsclient` program is implementation of a simple DNS client.  
The program gets a single command line argument which is the IP address of a DNS server, given
in dotted-decimal notation (`8.8.8.8` for example).  
Given a DNS server, the program asks the user to enter the requested host (domain) and if it seems that is is possible host name, it sends the server a query the requested domain.  
the programs keeps asking for more domain, until the user type `quit`.  
**`Iris Taubkin`** 208410969  
**`Omri Elad`** 204620702  


## Implementation
The program is divided into 3 modules:
| idx | name     | files                  | comments                                    |
| --- | -------- | ---------------------- | ------------------------------------------- |
| 1   | main     | main.c                 | Main function: loop over domains            |
| 2   | ns_utils | ns_utils.c, ns_utils.h | all the functions needed in main            |
| 3   | structs  | structs.h              | all the data structures needed for nsclient |


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
