#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#define LG_MESSAGE 256

int main(int argc, char *argv[]){
	int descripteurSocket;
	struct sockaddr_in sockaddrDistant;
	socklen_t longueurAdresse;

	int nb; /* nb d’octets écrits et lus */
    int lus;
    char messageRecu[LG_MESSAGE];
	char buffer[] = "";

	char ip_dest[16];
	int  port_dest;

	if (argc>2) { // si il y a au moins 2 arguments passés en ligne de commande, récupération ip et port
        strncpy(ip_dest,argv[1],16);
        sscanf(argv[2],"%d",&port_dest);
		printf("%d",  argc);
        if (strcmp(argv[3],"heure")){
            char buffer[]="heure"; // buffer stockant le message
        }
        else{
            if(strcmp(argv[3],"date")){
                char buffer[]="date"; // buffer stockant le message
            }
            else{
                printf("USAGE : %s ip port commande(heure/date)\n",argv[0]);
                exit(-1);
            }
        }
                
    }
    else{
        printf("USAGE : %s ip port commande(heure/date)\n",argv[0]);
        exit(-1);
    }
	// Crée un socket de communication
	descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
	// Teste la valeur renvoyée par l’appel système socket()
	if(descripteurSocket < 0){
		perror("Erreur en création de la socket..."); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée! (%d)\n", descripteurSocket);


	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant la machine distante)
	// Obtient la longueur en octets de la structure sockaddr_in
	longueurAdresse = sizeof(sockaddrDistant);
	// Initialise à 0 la structure sockaddr_in
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &sockaddrDistant
	memset(&sockaddrDistant, 0x00, longueurAdresse);
	// Renseigne la structure sockaddr_in avec les informations du serveur distant
	sockaddrDistant.sin_family = AF_INET;
	// On choisit le numéro de port d’écoute du serveur
	sockaddrDistant.sin_port = htons(port_dest);
	// On choisit l’adresse IPv4 du serveur
	inet_aton(ip_dest, &sockaddrDistant.sin_addr);

	// Débute la connexion vers le processus serveur distant
	if((connect(descripteurSocket, (struct sockaddr *)&sockaddrDistant,longueurAdresse)) == -1){
		perror("Erreur de connection avec le serveur distant...");
		close(descripteurSocket);
		exit(-2); // On sort en indiquant un code erreur
	}
	printf("Connexion au serveur %s:%d réussie!\n",ip_dest,port_dest);

 	// Envoi du message
	switch(nb = write(descripteurSocket, buffer, strlen(buffer))){
		case -1 : /* une erreur ! */
     			perror("Erreur en écriture...");
		     	close(descripteurSocket);
		     	exit(-3);
		case 0 : /* la socket est fermée */
			fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
			return 0;
		default: /* envoi de n octets */
			printf("Message %s envoyé! (%d octets)\n\n", buffer, nb);
	}

    lus = read(descripteurSocket, messageRecu, LG_MESSAGE*sizeof(char)); // ici appel bloquant
        switch(lus) {
            case -1 : /* une erreur ! */ 
                  perror("read"); 
                  close(descripteurSocket); 
                  exit(-5);
            case 0  : /* la socket est fermée */
                  fprintf(stderr, "La socket a été fermée par le client !\n\n");
                     close(descripteurSocket);
                     return 0;
            default:  /* réception de n octets */
                  printf("Message reçu : %s (%d octets)\n\n", messageRecu, lus);
        }

	// On ferme la ressource avant de quitter
	close(descripteurSocket);

	return 0;
}
