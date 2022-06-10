#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLEN 255

void die(char *);

int main(int argc, char **argv){
	if(argc < 2){
		die("no port specified");
	}

	/* port must be specified on argv */
	int port = atoi(argv[1]);
	char sendbuff[MAXLEN], recvbuff[MAXLEN];
	struct sockaddr_in server_ip_port;
	int sd, bytesent, byterecvd, server_ip_port_length = sizeof(server_ip_port);
	
	memset(sendbuff, 0, MAXLEN);
	memset(recvbuff, 0, MAXLEN);
	
	/* creo la socket */
	sd = socket(AF_INET, SOCK_STREAM, 0);

	if(sd < 0){
		die("socket() error");
	}

	printf("socket() ok.\n");

	/* indico le informazioni del server */	
	server_ip_port.sin_family = AF_INET;
	server_ip_port.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_ip_port.sin_port = htons(port);

	/* apro la connessione con il server, inizio quidni il 3hs */
	if(connect(sd, (struct sockaddr *) &server_ip_port, server_ip_port_length) < 0){
		die("connect() error");
	}

	printf("connect() ok.\n");

	printf("Inserisci stringa: ");
	gets(sendbuff);
	
	/* mando la stringa al server */
	bytesent = write(sd, sendbuff, MAXLEN);

	if(bytesent <= 0){
		die("sendto() error");
	}

	printf("%d byte sent.\n", bytesent);

	/* ricevo i dati dal server */	
	byterecvd = read(sd, recvbuff, MAXLEN);

	if(byterecvd <= 0){
		die("recvfrom() error");
	}

	printf("%d byte received.\n%s\n", byterecvd, recvbuff);
	
	/* chiudo la connessione */
	close(sd);	

	return 0;
}

void die(char *error){
	fprintf(stderr, "%s.\n", error);
	
	exit(1);
}
