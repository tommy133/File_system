/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "directorios.h"
#include <string.h> 

int main(int argc,char**argv){
    
    if(argc!=3){
		
		fprintf(stderr, "Error de sintaxis, correcto: $ ./mi_mkfs <nombre_fichero> <cantidad_bloques>\n");
		return -1;
	}

	unsigned int nbloques = atoi(argv[2]);
	unsigned int ninodos = nbloques/4;
		
	unsigned char buff[BLOCKSIZE];
	memset(buff,0,BLOCKSIZE); //Rellenamos de 0's el buffer
	
	bmount(argv[1]); //Montamos el dispositivo.
	
    for(int i=0; i<nbloques; i++){
		bwrite(i,buff);
	}
	
	if(initSB(nbloques,ninodos)<0)return -1;
	if(initMB()<0)return -1;
	if(initAI()<0)return -1;
    
    reservar_inodo('d', 7);
    
    bumount();

    return 0;
		
}
