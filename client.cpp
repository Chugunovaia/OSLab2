#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <string.h>


#define PORT 8080
char message[]="Hello, it's client";

using namespace std;
int main()
{
    int sock;
    struct sockaddr_in addr;
    sock=socket(AF_INET, SOCK_STREAM, 0);
    if (sock<0)
    {
        perror("socket");
        exit(1);
    }


    addr.sin_family=AF_INET;
    addr.sin_port=htons(PORT);
    addr.sin_addr.s_addr=htonl(INADDR_LOOPBACK);

    if (connect(sock,(struct sockaddr *)&addr, sizeof(addr))<0)
    {
        perror("connect");
        exit(2);
    }
    send(sock, message, sizeof(message),0);
    cout<<endl<<"Sent"<<endl;
    close(sock);
    return 0;
}
