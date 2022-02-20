/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "directorios.h"
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    int nbytes;
    int escritos;
    

    if (argc != 5){ //comprobamos la sintaxis
        fprintf(stderr, "Error. Sintaxis correcta: mi_escribir <disco> </ruta_fichero> <texto> <offset>");
        return -1;
    }
    //Recuperamos los parámetros

    char *disco = argv[1];
    char *camino = argv[2];
    char *texto = argv[3];
    int offset = atoi(argv[4]);


    nbytes = strlen(texto);
    printf("longitud texto: %d\n\n", nbytes);

    char buffer[nbytes];
    strcpy(buffer, texto);
    
    bmount(disco);
        
    if ((escritos=mi_write(camino, buffer, offset, nbytes))<0){
        fprintf(stderr, "Error de escritura en el fichero\n");
        return -1;
    }

    printf("Bytes escritos: %d\n\n",escritos);
    
    bumount();
    
    return 0;
    
}

