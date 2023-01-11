#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <stdbool.h>

#define taille_tab 9

struct morpion{
    /*Structure du morpion*/
    int tableaupos[taille_tab];
    char tableaujou[taille_tab];
    int coupjou;
    char gagnant;
};
/* Les fonctions appelées avec un pointeur sont les fonctions modifiant la structure morpion*/
void initmorp(struct morpion *morp){
    /*Fonction d'initialisation du morpion*/
    for (int i=0;i<taille_tab;i++){
        morp->tableaupos[i]=i+1;
        morp->tableaujou[i]=' ';
        morp->coupjou=0;
    }
    morp->gagnant=' ';
}

void affmorp(struct morpion morp){
    /*Fonction d'affichage du morpion (avec les cases jouées & celui avec les cases avec leur ID)*/
    printf("\n");
    for (int i=0;i<taille_tab;i++){
        if(i%3==0){
            printf("\n|");
        }
        printf("%i|",morp.tableaupos[i]);
    }
    printf("\n");
    for (int i=0;i<taille_tab;i++){
        if(i%3==0){
            printf("\n|");
        }
        printf("%c|",morp.tableaujou[i]);
    }
}

void jouercase(struct morpion *morp,int coord,bool xo){
    /* Fonction permettant de jour à la case demandée*/
    if(coord<1 || coord>9){

    }
    else{
    morp->coupjou++;
    morp->tableaupos[coord-1]=0;
    if(xo==1){
        morp->tableaujou[coord-1]='X';
    }
    else{
        morp->tableaujou[coord-1]='O';
    }
    }
}
int jouerobot(struct morpion morp){
    /* Fonction faisant jouer le serveur aléatoirement (retourn la case choisie)*/
    while(1){
        int a = rand()%10;
        while(a==0){
            a = rand()%10;
        }
        if(morp.tableaupos[a-1]!=0){
            return a;
        }
    }

    
}
int gagnant(struct morpion *morp){
    /* Fonction vérifiant s'il y a un gagnant*/
    for(int i=0;i<9;i+=3){ //Verif ligne
        if(morp->tableaujou[0+i]==morp->tableaujou[1+i] && morp->tableaujou[1+i]==morp->tableaujou[2+i] ){
            /* Vérifie que les cases identiques ne sont pas vides*/
            if((morp->tableaujou[0+i]!=' ' && morp->tableaujou[1+i]!=' ' && morp->tableaujou[2+i]!=' ')){
                morp->gagnant=morp->tableaujou[0+i]; 
                return 0;
            }
        }
    }
    for(int i=0;i<3;i++){ //Verif colonne
        if(morp->tableaujou[0+i]==morp->tableaujou[3+i] && morp->tableaujou[3+i]==morp->tableaujou[6+i] ){
           /* Vérifie que les cases identiques ne sont pas vides*/
           if(morp->tableaujou[0+i]!=' ' && morp->tableaujou[3+i]!=' ' && morp->tableaujou[6+i]!=' '){
                morp->gagnant=morp->tableaujou[0+i]; 
                return 0;
            } 
        }
    }
    //Verif diag 1
    if(morp->tableaujou[0]==morp->tableaujou[4] && morp->tableaujou[4]==morp->tableaujou[8] ){
           /* Vérifie que les cases identiques ne sont pas vides*/
           if(morp->tableaujou[0]!=' ' && morp->tableaujou[4]!=' ' && morp->tableaujou[8]!=' '){
                morp->gagnant=morp->tableaujou[0]; 
                return 0;
            }
    }
    //Verif diag 2
     if(morp->tableaujou[2]==morp->tableaujou[4] && morp->tableaujou[4]==morp->tableaujou[6] ){
           /* Vérifie que les cases identiques ne sont pas vides*/
           if(morp->tableaujou[2]!=' ' && morp->tableaujou[4]!=' ' && morp->tableaujou[6]!=' '){
                morp->gagnant=morp->tableaujou[2]; 
                return 0;
            } 
    }
    return 0;
}
void affgagnant(struct morpion morp){
    /* Fonction d'affichage du message de fin avec le gagnant */
    affmorp(morp);
    if('O'==morp.gagnant){
        printf("\nLe gagnant est le joueur 2 avec le symbole O");
    }
    if('X'==morp.gagnant){
        printf("\nLe gagnant est le joueur 1 avec le symbole X");
    }
    if(' '==morp.gagnant){
        printf("\nEgalite\n");
    }   
}

int main(){
    /* Main pour les tests*/
    struct morpion morp;
    struct morpion *mo=&morp;
    initmorp(mo);
    while((morp.coupjou!=9)){
        time_t t;
        srand(t);
        int caserobot=jouerobot(morp);
        int a=0;
        if(morp.coupjou%2==0){
            a=1;
        }
        else{
            a=0;
        }
        int w;
        jouercase(mo,caserobot,a);
        w=gagnant(mo);
        if(morp.coupjou!=9){
            affmorp(morp);
        }
        if(morp.gagnant!=' '){
            break;
        }
        
    }
    affgagnant(morp);
    printf("\n");
    return 0;
}