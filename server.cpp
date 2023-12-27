#include <iostream>
#include <netinet/ether.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#define  PORT 8080
//using namespace std;

//обработчик сигнала
volatile sig_atomic_t wasSigHup=0;
void sigHupHandler(int r)
{
wasSigHup=1;
}

int main()
{
    int counter=0;
//  создание сокета
    int sock=0;
    int listener=0;
    struct sockaddr_in addr;
    listener=socket(AF_INET, SOCK_STREAM,0);
    if (listener<0)
    {
        perror("socket");
        exit(1);
    }
//  заполнение структуры addr
    addr.sin_family=AF_INET;    //  семейство адресов
    addr.sin_port=htons(PORT);  //  номер порта
    addr.sin_addr.s_addr=htonl(INADDR_ANY); //  ip адрес (с любым)
//  явное связывание сокета с адресом
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))<0)
    {
        perror("bind");
        exit (2);
    }
// установка соединения
    listen(listener, 1); // очередь запросов на соединение
// регистрация обработчика сигнала
    struct sigaction sa;
    sigaction(SIGHUP, NULL, &sa);// тип сигнала, новый метод обр-ки, старый метод
    sa.sa_handler=sigHupHandler; // обработка сигнала
    sa.sa_flags|=SA_RESTART;
    sigaction(SIGHUP, &sa, NULL);
// блокировка сигнала
    sigset_t blockedMask, origMask;  //маски блокировки и исходная
    sigemptyset(&blockedMask); // пустое мн-во сигналов
    sigaddset(&blockedMask, SIGHUP); // добавляем sighup в мн-во
    sigprocmask(SIG_BLOCK, &blockedMask, &origMask); // рег. маску блока сигналов
// основной цикл
    int maxFd;
    while(counter==0)
    {
        fd_set fds; //мн-во файловых дескрипторов
        FD_ZERO(&fds); //опустошаем
        FD_SET(listener, &fds); //регистрируем фд
        maxFd=listener;// фд с набольшим номером
        if (sock>0)
        {
            FD_SET(sock, &fds); // регистрирум фд входящего
        }
        if (sock>maxFd)
        maxFd= sock;
        if (pselect(maxFd+1, &fds, NULL,NULL,NULL, &origMask)==-1)
        {
            if (errno==EINTR) //если прерван сигналом, принимаем его
            {
                if (wasSigHup==1) //сигнал приходил
                {
                wasSigHup=0;
                counter++;
                }



            } else if (!errno) // ошибка
            {
                perror("pselect");
                exit(3);
            }
        }

        if (FD_ISSET(listener, &fds))   // если не остался listener
        {
            sock=accept(listener,(struct sockaddr*)&addr,NULL);
            if (sock<0)
            {
                perror("accept");
                exit(4);
            }
        }


    }
    close(listener);
    return 0;
}
