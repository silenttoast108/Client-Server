#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

int main() {
    int contador = 0;
    int id = rand();
    int val_leer, sock_nuevo, sockfd, b;
    FILE *puntador;

    struct sockaddr_in dirección;
    dirección.sin_family = AF_INET;
    dirección.sin_addr.s_addr = INADDR_ANY;
    dirección.sin_port = 9999;

    socklen_t direclen = sizeof(dirección);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Falla de 'socket'\n");
        exit(EXIT_FAILURE);
    }

    if ((b = bind(sockfd, (struct sockaddr *)&dirección, sizeof(dirección))) < 0) {
        perror("Falla de 'bind'\n");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 1) < 0) {
        perror("Falla de 'listen'\n");
        exit(EXIT_FAILURE);
    }

    if ((sock_nuevo = accept(sockfd, (struct sockaddr *)&dirección, &direclen)) < 0) {
        perror("Falla de 'accept'\n");
        exit(EXIT_FAILURE);
    }

    char buf_lec[1024];
    char buf_lec2[1024];
    char buf_esc[1024];
    char buf_esc2[1024];
    char comando [1124];

    memset(buf_lec2, 0, sizeof(buf_lec2));
    memset(buf_esc2, 0, sizeof(buf_esc2));

    while ((recv(sock_nuevo, buf_lec, sizeof(buf_lec), 0)) > 0) {
        //printf("buf_lec: |%s|\n", buf_lec);

        strncat(buf_lec2, buf_lec, sizeof(buf_lec2) - strlen(buf_lec2) - 1);

        
        if (strstr(buf_lec, "\n") != NULL) {
            //printf("buf_lec2: |%s|\n", buf_lec2);

            switch(fork()) {
                case -1: /*Error*/
                    perror("Error en Fork");
                    _exit(0);

                case 0 : /*Hijo*/
                    sprintf(comando, "bash -c '%s' 2>&1", buf_lec2);
                    //printf("comando: |%s|\n", comando);
                    puntador = popen(comando, "r");
                    if (puntador == NULL) {
                        perror("Falla de 'popen'\n");
                        exit(EXIT_FAILURE);
                    }

                    while (fgets(buf_esc, sizeof(buf_esc), puntador))
                        strncat(buf_esc2, buf_esc, sizeof(buf_esc2) - strlen(buf_esc2) - 1);

                    fclose(puntador);

                    if (send(sock_nuevo, buf_esc2, sizeof(buf_esc2), 0)<0) {
                        perror("Falla de 'send'\n");
                        exit(EXIT_FAILURE);
                    }
                    _exit(0);
            }
            memset(buf_lec2, 0, sizeof(buf_lec2));
            memset(buf_esc2, 0, sizeof(buf_esc2));
            
            contador++;
        }    
    }
    close(sock_nuevo);
    close(sockfd);
}