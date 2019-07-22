#ifdef WIN32 /* si vous êtes sous Windows */
#include <winsock2.h>
#elif defined (linux) /* si vous êtes sous Linux */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
#else /* sinon vous êtes sur une plateforme non supportée */
#error not defined for this platform
#endif
#define PORT_IC 13008

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "video.h"

int main()
{
    #if defined (WIN32)
        WSADATA WSAData;
        int erreur = WSAStartup(MAKEWORD(2,2), &WSAData);
    #else
        int erreur = 0;
    #endif

    if(!erreur)
    {
        FILE* image[500] = {NULL};
        char name[9];

        for(int i=0; i<500; i++)
        {
            if(i<10)
                sprintf(name, "Capture/X000%d.jpg", i);
            else if(i<100)
                sprintf(name, "Captured/X00%d.jpg", i);
            else
                sprintf(name, "Captured/X0%d.jpg", i);

            printf("%s ", name);
            image[i] = fopen(name, "rb");
            if(image[i]!=NULL)
                printf("OK");
            else
                printf("ERROR!");
            printf("\n");
        }

        SOCKET sock, csock;
        SOCKADDR_IN sin, csin;

        char msg[] = {0, 0};
        char paquet[] = {0, 0, 0, 0, 0};
        unsigned char commande[] = {pow(2,2), 0};
        int sinsize = 0, cstChar = 128, index = 0, octet = 0;

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock != INVALID_SOCKET)
        {
            printf("Server launching succeeded\n");

            sin.sin_family = AF_INET;
            sin.sin_addr.s_addr = INADDR_ANY;
            sin.sin_port = htons(PORT_IC);
            bind(sock, (SOCKADDR*)&sin, sizeof(sin));
            listen(sock, 5);

            printf("Server now listening on port %d\n", htons(sin.sin_port));

            do
            {
                printf("\n\nWaiting for a connection...\n");
                erreur = 0;
                sinsize = sizeof(csin);
                csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
                if(csock != INVALID_SOCKET)
                {
                    printf("Connection accepted from %s on port %d\n", inet_ntoa(csin.sin_addr), htons(sin.sin_port));
                    printf("\no-----------o\n");
                    //printf("|           |\n");
                    printf("|   TRAME   |\n");
                    //printf("|           |\n");
                    printf("+-----------+\n");
                    printf("| [1] | [0] |\n");
                    printf("+-----------+\n");
                    while(erreur != SOCKET_ERROR && commande[0]>=pow(2,2))
                    {
                        printf("|  %d  |  %d  |\n", commande[0], commande[1]);
                        erreur = recv(csock, msg, sizeof(msg), 0);

                        paquet[0] = 0;
                        paquet[1] = 0;
                        paquet[2] = 0;
                        paquet[3] = 0;
                        paquet[4] = 0;
                        octet = 0;
                        int i = 1;
                        while(octet<4 && i == TRUE)
                        {
                            paquet[octet] = getc(image[index]);
                            if(paquet[octet] == EOF)
                            {
                                i = 0;
                                index++;
                                if(index > 500)
                                    index = 0;
                            }
                            else
                                octet++;
                        }
                        paquet[4] = octet + 1;
                        send(csock, paquet, sizeof(paquet), 0);

                        commande[0] = msg[0] + cstChar;
                        commande[1] = msg[1] + cstChar;
                    }
                    printf("o-----------o\n");
                    printf("\nClient %s closed connection\n", inet_ntoa(csin.sin_addr));
                }
            }while(commande[0] >= pow(2,2));

            //record(0);
        }

        else
            printf("Unable to start the server...");

        for(int i=0; i<500; i++)
        {
            fclose(image[i]);
        }

    }

    #if defined (WIN32)
        WSACleanup();
    #endif

    return 0;
}
