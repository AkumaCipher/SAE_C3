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
    int tableaupos[taille_tab];
    char tableaujou[taille_tab];
    int coupjou;
    char gagnant;
    
};

void initmorp(struct morpion *morp){
    
    for (int i=0;i<taille_tab;i++){
        morp->tableaupos[i]=i+1;
        morp->tableaujou[i]=' ';
        morp->coupjou=0;
        morp->gagnant='p';
    }
}

void affmorp(struct morpion morp){
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
void gagnant(struct morpion morp){
    int cpt;
    char ancien=morp.tableaujou[0];
    char gagnant;
    int win=0;
    //strcmp à la place des : "morp.tableaujou[i+b]==ancien"
    //strcpy à la place de : "ancien=morp.tableaujou[0]; & gagnant=morp.tableaujou[i+b];"
    for(int i=0;i<7;i+3){ //Verif ligne
        for(int b=0;b<3;b++){
            if(morp.tableaujou[i+b]==ancien){
                cpt++;
            }
            else{
                cpt=1;
            }
            if(cpt==3){
                gagnant=morp.tableaujou[i+b];
                win=1;
            }
        }   
    }
    for(int i=0;i<3;i++){ //Verif colonne
        for(int b=0;b<3;b++){
            if(morp.tableaujou[i+b*3]==ancien){
                cpt++;
            }
            else{
                cpt=1;
            }
            if(cpt==3){
                gagnant=morp.tableaujou[i+b];
                win=1;
            }
        }
    }
    for(int i=0;i<3;i++){ //Diagonale (0-->4-->8)
        int b=0;
        if(morp.tableaujou[i+(b*3)]==ancien){
                cpt++;
                b++;
            }    
        else{
                cpt=1;
            }
        if(cpt==3){
                gagnant=morp.tableaujou[i+(b*3)];
                win=1;
            }
        
    }

    for(int i=6;i>-1;i-2){ //Diagonale (6-->4-->2)
       if(morp.tableaujou[i]==ancien){
                cpt++;
            }    
        else{
                cpt=1;
            }
        if(cpt==3){
                gagnant=morp.tableaujou[i];
                win=1;
            } 
    }
    if(win==1){
        morp.gagnant=gagnant;
    }
}

void affgagnant(struct morpion morp){
    int a=0;
    if(strcmp("O",&morp.gagnant)==0){
        printf("Le gagnant est le joueur 2 avec le symbole O");
    }
    else{
        if(strcmp("X",&morp.gagnant)==0){
            printf("Le gagnant est le joueur 1 avec le symbole X");
        }
        else{
            printf("Egalite");
        }
    
    }
}

int main(){
    srand(time(NULL));
    struct morpion morp;
    struct morpion *mo=&morp;
    initmorp(mo);
    while(morp.coupjou!=9 || morp.gagnant=='p'){
        int caserobot=jouerobot(morp);
        int a=0;
        if(morp.coupjou%2==0){
            a=1;
        }
        else{
            a=0;
        }
        jouercase(mo,caserobot,a);
        gagnant(morp);
        if(morp.coupjou!=9){
            affmorp(morp);
        }
    }
    affmorp(morp);
    affgagnant(morp);
    printf("\n");
    return 0;
}