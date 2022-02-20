/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "ficheros.h"

int main(int argc, char **argv) {

if(argc<4){
    perror("SINTAXIS INCORRECTA");
    perror("SINTAXIS CORRECTA : permitir <nombre_dispositivo> <ninodo> <permisos>");
return -1;
}

int ninodo=atoi(argv[2]);
int permisos=atoi(argv[3]);

bmount(argv[1]);
mi_chmod_f(ninodo, permisos);
bumount();
    
return 0;


}
