#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <string.h>		/* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h>	/* pour htons et inet_aton */
#include <stdbool.h>

#define PORT IPPORT_USERRESERVED // = 5000 (ports >= 5000 réservés pour usage explicite)

#define LG_MESSAGE 256

#define taille_tab 9

struct morpion
{
	/*Structure du morpion*/
	int tableaupos[taille_tab];
	char tableaujou[taille_tab];
	int coupjou;
	char gagnant;
};
/* Les fonctions appelées avec un pointeur sont les fonctions modifiant la structure morpion*/
void initmorp(struct morpion *morp)
{
	/*Fonction d'initialisation du morpion*/
	for (int i = 0; i < taille_tab; i++)
	{
		morp->tableaupos[i] = i + 1;
		morp->tableaujou[i] = ' ';
		morp->coupjou = 0;
	}
	morp->gagnant = ' ';
}

void affmorp(struct morpion morp)
{
	/*Fonction d'affichage du morpion (avec les cases jouées & celui avec les cases avec leur ID)*/
	printf("\n");
	for (int i = 0; i < taille_tab; i++)
	{
		if (i % 3 == 0)
		{
			printf("\n|");
		}
		printf("%i|", morp.tableaupos[i]);
	}
	printf("\n");
	for (int i = 0; i < taille_tab; i++)
	{
		if (i % 3 == 0)
		{
			printf("\n|");
		}
		printf("%c|", morp.tableaujou[i]);
	}
}

void jouercase(struct morpion *morp, int coord, bool xo)
{
	/* Fonction permettant de jour à la case demandée*/
	if (coord < 1 || coord > 9)
	{
	}
	else
	{
		morp->coupjou++;
		morp->tableaupos[coord - 1] = 0;
		if (xo == 1)
		{
			morp->tableaujou[coord - 1] = 'X';
		}
		else
		{
			morp->tableaujou[coord - 1] = 'O';
		}
	}
}
int jouerobot(struct morpion morp)
{
	/* Fonction faisant jouer le serveur aléatoirement (retourn la case choisie)*/
	while (1)
	{
		int a = rand() % 10;
		while (a == 0)
		{
			a = rand() % 10;
		}
		if (morp.tableaupos[a - 1] != 0)
		{
			return a;
		}
	}
}
int gagnant(struct morpion *morp)
{
	/* Fonction vérifiant s'il y a un gagnant*/
	for (int i = 0; i < 9; i += 3)
	{ // Verif ligne
		if (morp->tableaujou[0 + i] == morp->tableaujou[1 + i] && morp->tableaujou[1 + i] == morp->tableaujou[2 + i])
		{
			/* Vérifie que les cases identiques ne sont pas vides*/
			if ((morp->tableaujou[0 + i] != ' ' && morp->tableaujou[1 + i] != ' ' && morp->tableaujou[2 + i] != ' '))
			{
				morp->gagnant = morp->tableaujou[0 + i];
				return 0;
			}
		}
	}
	for (int i = 0; i < 3; i++)
	{ // Verif colonne
		if (morp->tableaujou[0 + i] == morp->tableaujou[3 + i] && morp->tableaujou[3 + i] == morp->tableaujou[6 + i])
		{
			/* Vérifie que les cases identiques ne sont pas vides*/
			if (morp->tableaujou[0 + i] != ' ' && morp->tableaujou[3 + i] != ' ' && morp->tableaujou[6 + i] != ' ')
			{
				morp->gagnant = morp->tableaujou[0 + i];
				return 0;
			}
		}
	}
	// Verif diag 1
	if (morp->tableaujou[0] == morp->tableaujou[4] && morp->tableaujou[4] == morp->tableaujou[8])
	{
		/* Vérifie que les cases identiques ne sont pas vides*/
		if (morp->tableaujou[0] != ' ' && morp->tableaujou[4] != ' ' && morp->tableaujou[8] != ' ')
		{
			morp->gagnant = morp->tableaujou[0];
			return 0;
		}
	}
	// Verif diag 2
	if (morp->tableaujou[2] == morp->tableaujou[4] && morp->tableaujou[4] == morp->tableaujou[6])
	{
		/* Vérifie que les cases identiques ne sont pas vides*/
		if (morp->tableaujou[2] != ' ' && morp->tableaujou[4] != ' ' && morp->tableaujou[6] != ' ')
		{
			morp->gagnant = morp->tableaujou[2];
			return 0;
		}
	}
	return 0;
}
void affgagnant(struct morpion morp)
{
	/* Fonction d'affichage du message de fin avec le gagnant */
	affmorp(morp);
	if ('O' == morp.gagnant)
	{
		printf("\nLe gagnant est le joueur 2 avec le symbole O");
	}
	if ('X' == morp.gagnant)
	{
		printf("\nLe gagnant est le joueur 1 avec le symbole X");
	}
	if (' ' == morp.gagnant)
	{
		printf("\nEgalite\n");
	}
}


int main(int argc, char *argv[])
{

	int socketEcoute;

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	int socketDialogue;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */
	int ecrits, lus;			  /* nb d’octets ecrits et lus */
	int retour;

	char buffer;

	char start = ' ';

	int nb;

	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0);
	// Teste la valeur renvoyée par l’appel système socket()
	if (socketEcoute < 0)
	{
		perror("socket"); // Affiche le message d’erreur
		exit(-1);		  // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale

	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	longueurAdresse = sizeof(pointDeRencontreLocal);
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &pointDeRencontreLocal
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse);

	pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(PORT);			   // = 5000 ou plus

	// On demande l’attachement local de la socket
	if ((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0)
	{
		perror("bind");
		exit(-2);
	}
	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if (listen(socketEcoute, 5) < 0)
	{
		perror("listen");
		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");

	while (1)
	{
		struct morpion morp;
		struct morpion *mo = &morp;
		initmorp(mo);
		start = ' ';
		// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment !
		memset(messageRecu, 0x00, LG_MESSAGE * sizeof(char));
		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");

		// c’est un appel bloquant
		socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogue < 0)
		{
			perror("accept");
			close(socketDialogue);
			close(socketEcoute);
			exit(-4);
		}

		printf("Débuter une partie ? (start/no)\n");
		scanf("%c", &start);
		if (strcmp(&start,"no") == 0)
		{
			printf("Éteinte du jeu.\n");
			exit(-5);
		}
		else
		{
			printf("Début de jeu !\n");
		}
		nb = write(socketDialogue, &start, 1);
		while (1)
		{
			time_t t;
			srand(t);
			lus = read(socketDialogue, &buffer, 1);
			switch (lus)
			{
			case -1:
				perror("read");
				close(socketDialogue);
				exit(-6);
			case 0:
				fprintf(stderr, "La socket a été fermée par le client !\n\n");
				close(socketDialogue);
				return 0;
			default:
				if (morp.coupjou != 9)
				{	
					int cases;
					sscanf(&buffer,"%d",&cases);
					jouercase(mo, cases, 1);
					affmorp(morp);
					gagnant(mo);
					if(morp.gagnant=='X'){
						strcpy(&buffer,"Xwins");
					}
					else{
						int caser = jouerobot(morp);
						printf("\n\nCase choisie par l'IA : %d\n\n", caser);
						jouercase(mo, caser, 0);
						gagnant(mo);
						if(morp.gagnant=='O'){
							char message_envoyer;
							strcpy(&message_envoyer,"Owins");
							int longu=strlen(&message_envoyer);
							strcpy(message_envoyer,caser);
						}
					}
				}
				else
				{
					strcpy(&buffer,"Xend");
					break;
				}
			}
			nb = write(socketDialogue, &buffer, 1);
			}
			
			
			
			
			
	}
	printf("Toutes les cases ont étés remplis, fin du jeu !");
	close(socketDialogue);
	close(socketEcoute);
	return 0;
}