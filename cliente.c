#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    char ip [50];
    strcpy(ip, argv[1]);
    int val_leer, sock_nuevo, clientfd;
    char buf_esc[1024];
    char buf_lec[1024];
    char buf_hist[1024];
    int cabado = 0;
    char fichera_hist [100] = "historia.txt";

    FILE *puntador2;
    FILE *puntador;
    if ((puntador = fopen(fichera_hist, "a")) == NULL) {
        perror("Falla de 'fopen'\n");
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in dirección;
    dirección.sin_family = AF_INET;
    dirección.sin_port = 9999;

     if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Falla de 'socket'");
        return -1;
    }

    if (inet_pton(AF_INET, ip, &dirección.sin_addr)<= 0) {
        printf("\nDirección Invalido\n");
        return -1;
    }

    if ((sock_nuevo = connect(clientfd, (struct sockaddr*)&dirección, sizeof(dirección)))< 0) {
        printf("Falla de 'connect'\n");
        return -1;
    }

    //send(clientfd, "echo \"Conexión establecida\"", strlen("echo \"Conexión establecida\""), 0);

    while(cabado == 0) {

        printf("Introduzca un comando para que el servidor lo realiza\n");
        printf("Para acceder la historia, introduzca 'historia'\n");
        printf("Para terminar la programa, introduzca 'FIN'\n");
        printf("\n");
        printf("$ usario ");
        fgets(buf_esc, sizeof(buf_esc), stdin);
        printf("\n");

        if (strstr(buf_esc, "FIN")!=NULL) {
            cabado=1;
        }

        if (strstr(buf_esc, "historia")==NULL) {
            fprintf(puntador, "usario$ %s\n", buf_esc);           

            if ((send(clientfd, buf_esc, sizeof(buf_esc), 0)) < 0) {
                printf("Falla de 'send'\n");
                return -1;
            }

            if ((recv(clientfd, buf_lec, sizeof(buf_lec), 0)) < 0) {
                printf("Falla de 'recv'\n");
                return -1;
            }

            if (cabado == 0) {
                printf("%s\n", buf_lec); 
                fprintf(puntador, "%s\n", buf_lec);
                fflush(puntador);
            }
        } else {    
            if ((puntador2 = fopen(fichera_hist, "r")) == NULL) {
                perror("Falla de 'fopen'\n");
                exit(EXIT_FAILURE);
            } 
            while(fgets(buf_hist, sizeof(buf_hist), puntador2)) {
                    printf("%s", buf_hist);
                }
        }
    }
    close(clientfd);
    fclose(puntador);
    system("rm historia.txt");
}