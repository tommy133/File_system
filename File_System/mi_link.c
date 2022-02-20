/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include <stdio.h>
#include "directorios.h"

int main(int argc, char **argv) {

	if (argc != 4){ //comprobamos la sintaxis
	    fprintf(stderr, "Error. Sintaxis correcta: ./mi_link <disco> </ruta_fichero_original> </ruta_enlace>");
	    return -1;
	}

	//Recuperamos los parámetros

	char *disco = argv[1];
	char *camino1 = argv[2];
	char *camino2 = argv[3];

	bmount(disco);

	if (mi_link(camino1, camino2) < 0){
		fprintf(stderr, "Error al crear el enlace\n");
		return -1;
	}

	printf("Enlace físico creado con éxito\n");

	bumount();

	return EXIT_SUCCESS;

}
