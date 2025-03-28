/*
ISTRUZIONI:
Creare una comunicazione tra server e client, in cui il client si connette al server e gli invia un messaggio, ricevendo come risposta una frase gentile.


PARAMETRI:
IP: localhost
PORT: 5700
*/
//includes
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



int initSocket(int domain, int type, int protocol, int IP, int port,struct sockaddr_in** addr_r);
void comunicate(int fd,char* IP,int port);

int main(){

	//Inizializzo le variabili
	int sockfd,clientfd;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in* addr;

	//imposto la socket con:
	// - IPv4 =  AF_INET
	// - TCP = SOCK_STREAM
	// - Standard protocol = 0
	// - INADDR_ANY = qualsiasi indirizzo
	// - PORTA = 5700
	sockfd = initSocket(AF_INET,SOCK_STREAM,0,INADDR_ANY,PORT,&addr);

	//apriamo la comunicazione in lettura
	while(1){

		//metto il server in ascolto e controllo un possibile errore
		if(listen(sockfd, 3) < 0){
			perror("Listen");
			exit(EXIT_FAILURE);
		}

		//accetto la richiesta di connessione e controllo un possibile errore
		if((clientfd = accept(sockfd,(struct sockaddr*)addr,&addrlen)) < 0){
			perror("Accept");
			exit(EXIT_FAILURE);
		}

		//creo un processo figlio per gestire la comunicazione con il client
		if(!fork()){
			comunicate(clientfd,inet_ntoa(addr->sin_addr),ntohs(addr->sin_port));
			exit(EXIT_SUCCESS);
		}

	}

	return 0;
}

int initSocket(int domain, int type, int protocol, int IP, int port,struct sockaddr_in** addr_r){

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

	//binding dei valori
	if(bind(fd,(struct sockaddr*)addr,sizeof(struct sockaddr_in)) < 0){
		perror("Binding");
		exit(EXIT_FAILURE);
	}

	*addr_r = addr;
	return fd;
}


void comunicate(int fd,char* IP,int port){
	srand(time(NULL));
	//la comunicazione con il client si basa:
	//il client invia un messaggio
	//il server lo stampa a schermo e risponde con un messaggio "Gentile" scelto casualmente
	//il client riceve il messaggio e lo stampa a schermo

	//Inizializzazione delle variabili
	char *frasi[] = {"La ringrazio sinceramente.","E' un piacere poterle essere d'aiuto.","Le sono molto grato per la sua disponibilità.","Le auguro una giornata piacevole.","E' un onore fare la sua conoscenza."};

	char buffer[MAX_SIZE];
	buffer[MAX_SIZE-1] = '\0';


	//leggo dalla socket
	if(read(fd,buffer,MAX_SIZE-1) < 0){
		perror("Errore Read");
		exit(EXIT_FAILURE);
	}

	//stampo a schermo il valore letto
	printf("Messaggio del client [%s] [%d]: %s\n",IP,port,buffer);

	//invio al client la frase gentile
	//estraggo un numero casuale
	int i = rand()%5;

	//scrivo il messaggio al client
	if(write(fd,frasi[i],MAX_SIZE-1) < 0){
		perror("Errore Write");
		exit(EXIT_FAILURE);
	}
}
