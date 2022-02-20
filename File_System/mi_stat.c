/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "directorios.h"

int main(int argc, char **argv) {  
	char *disco = argv[1];  
	char *camino = argv[2];
	struct STAT meta;
	struct tm *ts;
	char atime[80];
    char mtime[80];
    char ctime[80];


    if (argc != 3) {
        fprintf(stderr, "Error. Sintaxis correcta: mi_stat <disco> </ruta>\n");
        return -1;
    }   

	bmount(disco);

	int ninodo;

	if ( (ninodo = mi_stat(camino, &meta)) < 0){
		fprintf(stderr, "Error al leer la información del fichero/directorio\n");
		return -1;
	}

	printf("Fichero: %s\n", camino);
	printf("Nº de inodo: %d\n", ninodo);
	printf("Tipo: %c\n", meta.tipo);
	printf("Tamaño: %d\t Bloques:%d\t Bloque E/S: \n", meta.tamEnBytesLog, meta.numBloquesOcupados);
	printf("Acceso: %d\n", meta.permisos);
	printf("Enlaces: %d\n", meta.nlinks);
	ts = localtime(&meta.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&meta.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&meta.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    printf("atime: %s\n", atime);
    printf("ctime: %s\n", ctime);
    printf("mtime: %s\n", mtime);

	bumount();

	return EXIT_SUCCESS;
}