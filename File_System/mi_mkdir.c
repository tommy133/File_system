/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "directorios.h"

int main(int argc, char **argv) {  
	char *disco = argv[1];  
	char *camino = argv[3];
    char permisos = atoi(argv[2]); 

    if (argc != 4) {
        fprintf(stderr, "Error. Sintaxis correcta: mi_mkdir <disco> <permisos> </ruta>\n");
        return -1;
    }   
    
    if (permisos < 0 || permisos > 7){
        fprintf(stderr, "Error. Permisos debe ser un nº válido (0-7)\n");
        return -1;
    } 
       
    bmount(disco);

    if ( mi_creat(camino, permisos) < 0){
        fprintf(stderr, "Error al crear el directorio\n");
    }
      
    bumount();  
      
    return EXIT_SUCCESS;    
}
