/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "directorios.h"

int main(int argc, char **argv) {  
	char *disco = argv[1];  
	char *camino = argv[3];
    char permisos = atoi(argv[2]); 

    if (argc != 4) {
        fprintf(stderr, "Error. Sintaxis correcta: mi_touch <disco> <permisos> </ruta>\n");
        return -1;
    }   
    
    if (permisos < 0 || permisos > 7){
        fprintf(stderr, "Error. Permisos debe ser un nº válido (0-7)\n");
        return -1;
    } 
       
    bmount(disco);

    if (camino[strlen(camino)-1]!='/') {//mi_touch no puede crear directorios
        if (mi_creat(camino, permisos) < 0){
            fprintf(stderr, "Error al crear el fichero");
            return -1;
        }
	    printf("mi_touch: fichero creado con exito\n");
    } else { 
        fprintf(stderr, "mi_touch: el camino no es un fichero\n");
        return -1;
    }    

    bumount();  
      
    return EXIT_SUCCESS;    
}
