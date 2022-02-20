/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "directorios.h"

int main(int argc, char **argv) {  
	char *disco = argv[1];  
	char *camino = argv[3];
	unsigned char permisos = atoi(argv[2]);

    if (argc != 4) {
        fprintf(stderr, "Error. Sintaxis correcta: mi_chmod <disco> <permisos> </ruta>\n");
        return -1;
    }   

    if(permisos > 7){
		printf("mi_chmod()--> Error. El rango de permisos es [0..7]\n");
		return -1;
	}

	bmount(disco);

	if (mi_chmod(camino, permisos) < 0) return -1;

	bumount();

	return EXIT_SUCCESS;
}