#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLEN 255
#define MAXCON 5

void die(char *);

int main(int argc, char **argv){
    int i;

    /* variabili per creare la socket */
    int port = atoi(argv[1]);
    int server_socket;
    struct sockaddr_in addr; 
    int addr_len = sizeof(addr);
    char read_buffer[MAXLEN], write_buffer[MAXLEN];
    int rec_byte, send_byte;

    /* variabili per fare le select */ 
    int peer_socket;
    int maxsd; 
    int select_res;
    fd_set readset, tmpset;
    struct timeval tv;

    /* azzero i buffer */
    memset(read_buffer, 0, MAXLEN);
    memset(write_buffer, 0, MAXLEN);

    /* creo la socket TCP */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(server_socket < 0){
        die("socket() error");
    }

    printf("socket() ok.\n");

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(port);

    if(bind(server_socket, (struct sockaddr *) &addr, addr_len) < 0){
        die("bind() error");
    }

    printf("bind() ok.\n");

    if(listen(server_socket, MAXCON) != 0){
        die("listen() error");
    }

    printf("listen() ok.\n");

    printf("\nserver on port %d\n\n", port);

    /*  
        inizializzo l'array delle socket, 
        inserisco la socket server e 
        setto il massimo con il suo valore
    */
    FD_ZERO(&readset);
    FD_SET(server_socket, &readset);
    maxsd = server_socket;

    /* inizializzo tv */
    tv.tv_sec = 20;
    tv.tv_usec = 0;

    /* ciclo principale per gestire i client */
    while(1){
        /* salvo l'array per ripristinarlo */
        memcpy(&tmpset, &readset, sizeof(readset));

        /* faccio la prima select dove estraggo la socket attiva */
        select_res = select(maxsd + 1, &tmpset, NULL, NULL, &tv);

        if(select_res < 0){
            die("select() error");
        }

        /* se la server socket è attiva gestisco i client */
        if(FD_ISSET(server_socket, &tmpset)){
            /* accetto la connessione */
            peer_socket = accept(server_socket, (struct sockaddr *) &addr, &addr_len);

            if(peer_socket < 0){
                die("accept() error");
            }

            printf("accept() ok on peer_socket %d.\n", peer_socket);

            /* acceta la connessione, metto la socket del client nell'array */
            FD_SET(peer_socket, &readset);
            maxsd = (maxsd > peer_socket ? maxsd : peer_socket);

            FD_CLR(server_socket, &tmpset);
        }

        /* gestisco tutti i client nell'array */
        for(i = 0; i < maxsd + 1; i++){
            /* se il client è nell'array dei client da servire lo gestisco facendo read e write */
            if(FD_ISSET(i, &tmpset)){
                rec_byte = read(i, read_buffer, MAXLEN);

                if(rec_byte <= 0){
                    die("read() error");
                }

                printf("\nread() ok from peer_socket: %d.\n%d byte received: %s\n\n", i, rec_byte, read_buffer);

                strcpy(write_buffer, read_buffer);

                send_byte = write(i, write_buffer, MAXLEN);

                if(send_byte <= 0){
                    die("write() error");
                }

                printf("write() ok on peer_socket: %d.\n", i);

                /* dopo averlo gestito chiudo la socket del client */
                close(i);

                /* tolgo la socket dall'array */
                FD_CLR(i, &readset);
            }
        }
    }

    close(server_socket);

    return 0;
}

void die(char *error){
	fprintf(stderr, "%s.\n", error);
	
	exit(1);
}