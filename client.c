/*
ISTRUZIONI:
Creare una comunicazione tra server e client, in cui il client si connette al server e gli invia un messaggio, ricevendo come risposta una frase gentile.


PARAMETRI:
IP: localhost
PORT: 5700
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 5700
#define MAX_SIZE 1024
char IP[] =  "127.0.0.1";


int initSocket(int domain, int type, int protocol, in_addr_t IP, int port,struct sockaddr_in** addr_r);
in_addr_t ip_to_addr_t(char *ip);
void comunicate(int fd);

int main(){

	//Inizializzo le variabili
	struct sockaddr_in* addr;
	int status,sockfd;

	//Inizializzo il socket
	sockfd = initSocket(AF_INET,SOCK_STREAM,0,ip_to_addr_t(IP),PORT,&addr);

	//Provo a connettermi
	if((status = connect(sockfd,(struct sockaddr*)addr,sizeof(struct sockaddr_in))) < 0){
		perror("Connect");
		exit(EXIT_FAILURE);
	}

	comunicate(sockfd);

	return 0;
}

in_addr_t ip_to_addr_t(char *ip){

	struct in_addr addr;

    if (inet_pton(AF_INET, ip, &addr) != 1) {
        
        return 0;
    }
    
    return addr.s_addr;
}

void comunicate(int fd){

	//inizializzazione delle variabili
	char buffer[MAX_SIZE];
	buffer[MAX_SIZE-1] = '\0';

	printf("Inserisci un messaggio :");
	scanf("%s",buffer);

	//invio un messaggio al server
	if(write(fd,buffer,MAX_SIZE-1) < 0){
		perror("Errore Write");
		exit(EXIT_FAILURE);
	}

	//ricevo un messaggio dal server
	if(read(fd,buffer,MAX_SIZE-1) <= 0){
		perror("Errore Read");
		exit(EXIT_FAILURE);
	}

	printf("Messaggio ricevuto dal server : %s\n",buffer);
}

int initSocket(int domain, int type, int protocol, in_addr_t IP, int port,struct sockaddr_in** addr_r){

	//creazione della socket e controllo errori
	int fd = socket(domain, type, protocol);

	//alloco dinamicamente
	struct sockaddr_in* addr;
	addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));


	if(fd < 0){
		perror("Socket");
		exit(EXIT_FAILURE);
	}

	//Imposto i valori nella struct
	addr->sin_family = domain;
	addr->sin_addr.s_addr = IP;
	addr->sin_port = htons(port);

	*addr_r = addr;
	return fd;
}