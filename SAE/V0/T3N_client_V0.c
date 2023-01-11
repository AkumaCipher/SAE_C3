#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>     /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h>  /* pour htons et inet_aton */
#include <stdbool.h>

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

    bool start = false;

    int descripteurSocket;
    struct sockaddr_in sockaddrDistant;
    socklen_t longueurAdresse;

    int nb; /* nb d’octets écrits et lus */
    int lus;
    char messageRecu[LG_MESSAGE];
    int buffer;

    char ip_dest[16];
    int port_dest;

    if (argc > 1)
    { // si il y a au moins 3 arguments passés en ligne de commande, récupération ip et port
        strncpy(ip_dest, argv[1], 16);
        sscanf(argv[2], "%d", &port_dest);
    }
    else
    {
        printf("USAGE : %s ip port\n", argv[0]);
        exit(-1);
    }

    while (1)
    {
        // Crée un socket de communication

        descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
        // Teste la valeur renvoyée par l’appel système socket()
        if (descripteurSocket < 0)
        {
            perror("Erreur en création de la socket..."); // Affiche le message d’erreur
            exit(-1);                                     // On sort en indiquant un code erreur
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
        if ((connect(descripteurSocket, (struct sockaddr *)&sockaddrDistant, longueurAdresse)) == -1)
        {
            perror("Erreur de connection avec le serveur distant...");
            close(descripteurSocket);
            exit(-2); // On sort en indiquant un code erreur
        }
        printf("Connexion au serveur %s:%d réussie!\n", ip_dest, port_dest);

        start = false;

        lus = read(descripteurSocket, messageRecu, LG_MESSAGE * sizeof(char)); // ici appel bloquant
        switch (lus)
        {
        case -1: /* une erreur ! */
            perror("read");
            close(descripteurSocket);
            exit(-3);
        case 0: /* la socket est fermée */
            fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
            close(descripteurSocket);
            return 0;
        default:
            if (*messageRecu == 'y')
            {
                start = true;
                printf("Début de la partie !\n");
            }
        }
        // Création du morpion pour ce socket.
        struct morpion morp;
        struct morpion *mo = &morp;
        initmorp(mo);
        while (start == true)
        {
            time_t t;
            srand(t);

            affmorp(morp);
            if (morp.coupjou == 9)
            {
                break;
            }
            printf("\n\nChoisissez votre case : ");
            scanf("%d", &buffer);
            jouercase(mo, buffer, 1);
            nb = write(descripteurSocket, &buffer, 1);

            switch (nb)
            {
            case -1: /* une erreur ! */
                perror("Erreur en écriture...");
                close(descripteurSocket);
                exit(-4);
            case 0: /* la socket est fermée */
                fprintf(stderr, "La socket a été fermée par le client !\n\n");
                return 0;
            default:
                printf("\nCase choisie et envoyee : %d\n", buffer);
            }
            if (morp.coupjou == 9)
            {
                break;
            }
            lus = read(descripteurSocket, &buffer, 1);
            switch (lus)
            {
            case -1:
                perror("read");
                close(descripteurSocket);
                exit(-5);
            case 0:
                fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
                close(descripteurSocket);
            default:
                jouercase(mo, buffer, 0);
            }
        }
    }

    printf("Toutes les cases ont étés remplis, fin du jeu !");
    // On ferme la ressource avant de quitter
    close(descripteurSocket);

    return 0;
}
