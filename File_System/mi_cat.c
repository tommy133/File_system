/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "directorios.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
    
    struct inodo inodo;
    int tam_buf = 1500*BLOCKSIZE;
    char buffer[tam_buf];
    int offset;
    char string[128];
    
    if (argc != 3){
        fprintf(stderr, "Error. Sintaxis correcta: mi_cat <disco> </ruta_fichero>\n");
        return -1;
    }
    
    char *disco = argv[1];
    char *camino = argv[2];

    bmount(disco);
    
    offset = 0;
    int leidos, total_leidos=0;

    memset (buffer, 0, tam_buf);
    leidos = mi_read(camino, buffer, offset, tam_buf);

    while (leidos > 0){
        write(1, buffer, leidos);
        total_leidos+=leidos;
        offset += tam_buf;
        memset (buffer, 0, tam_buf);
        leidos=mi_read(camino, buffer, offset, tam_buf);
    }
    
    sprintf(string, "\ntotal_leidos %d\n", total_leidos);
    write(2, string, strlen(string));

    int res;
    unsigned int p_inodo_dir = 0, p_inodo, p_entrada;
    
    if ( (res = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 7)) < 0){
        mostrar_error_buscar_entrada(res);
        return res;;
    }
    leer_inodo(p_inodo, &inodo);

    sprintf(string, "tamEnBytesLog %d\n", inodo.tamEnBytesLog);
    write(2, string, strlen(string));
    
    bumount();
    
    return 0;
    
}
