/*********************************************************/

/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/

/*********************************************************/

#include "directorios.h"
#define TAMFILA 100
#define TAMBUFFER (TAMFILA*1000) 

//suponemos un máx de 1000 entradas, aunque debería ser SB.totInodos
void extract_buf(char *buffer, int n){
	char entry[LONG_ENTRADA];
	char *start, *end;
	start = buffer;
    
    printf("Tipo\t\tPermisos\t\tmTime\t\tTamaño\t\tNombre\n");
	for (int i=0; i < n; i++){
        memset(entry,0,sizeof(entry));
        end = strchr(start, '|');
    	strncpy(entry, start, end-start);
        start = end+1;
    	printf("%s\n", entry);
    }
}
int main(int argc, char **argv) {  
	char *disco = argv[1];  
	char *camino = argv[2];
    if (argc != 3) {
        fprintf(stderr, "Error. Sintaxis correcta: mi_ls <disco> </ruta_directorio>\n");
        return -1;
    }   
    if(camino[strlen(camino)-1] != '/'){ //Llevar si feim millora mi_ls

		printf("mi_ls: %s corresponde al path de un fichero\n",camino);
		return -1;
	} 
    bmount(disco);
    char buffer[TAMBUFFER];
    int n_elem;
    memset(buffer,0,sizeof(buffer));
    if ( (n_elem = mi_dir(camino, buffer)) < 0){
    	fprintf(stderr, "Error al cargar la información del fichero/directorio\n");
    	return -1;
    }

    printf("Total: %d\n", n_elem);
    extract_buf(buffer, n_elem);
    bumount();

    return EXIT_SUCCESS;
}


