#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#define LG_MESSAGE 256

void lire_date(char *date){
	FILE *fpipe;

	fpipe = popen("date +'%A %d %B %Y'","r");
	if (fpipe == NULL){
		perror("popen");
		exit(-1);
	}
	fgets(date,LG_MESSAGE,fpipe);
	pclose(fpipe);
}
void lire_heure(char *heure){
	FILE *fpipe;

	fpipe = popen("date +'%X'","r");
	if (fpipe == NULL){
		perror("popen");
		exit(-1);
	}
	fgets(heure,LG_MESSAGE,fpipe);
	pclose(fpipe);
}

int main(){
    char d[40];
    lire_heure(d);
    printf("%s\n",d);
}