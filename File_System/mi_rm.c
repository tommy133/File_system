/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/

#include <stdio.h>
#include "directorios.h"

int main (int argc, char **argv) {

	if(argc != 3){
		fprintf(stderr, "Error. Sintaxis correcta: mi_rm <disco> </ruta>\n");
		return -1;
	}

    char *disco = argv[1];  
	char *camino = argv[2];

    if (strcmp(camino,"/")== 0){
        fprintf(stderr, "Error: No se puede borrar la raiz del sistema de ficheros!!\n");
        return -1;
    }

    bmount(disco);

    if (mi_unlink(camino) < 0){
        fprintf(stderr, "Error al borrar el archivo\n");
        return -1;
    }

    printf("Archivo borrado correctamente\n");
    return 0;
    
}