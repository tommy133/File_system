/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "ficheros.h"

int main(int argc, char **argv) {

int ninodo;
int nbytes;
struct tm *ts;
char atime[80];
char mtime[80];
char ctime[80];
struct STAT p_stat;


if(argc<4){
    perror("SINTAXIS INCORRECTA");
    perror("SINTAXIS CORRECTA : truncar <nombre_dispositivo> <ninodo> <nbytes>");
return -1;
}

ninodo = atoi(argv[2]);
nbytes = atoi(argv[3]);

bmount(argv[1]);

if (nbytes == 0){
	liberar_inodo(ninodo);
} else {
	mi_truncar_f(ninodo, nbytes);
}

mi_stat_f(ninodo,&p_stat);
printf("\nDATOS INODO 1:\n");
printf("tipo=%c\n", p_stat.tipo);
printf("permisos=%d\n", p_stat.permisos);
ts = localtime(&p_stat.atime);
strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
ts = localtime(&p_stat.mtime);
strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
ts = localtime(&p_stat.ctime);
strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
printf("atime: %s\n",atime);
printf("mtime: %s\n",mtime);
printf("ctime: %s\n",ctime);
printf("nlinks: %d\n", p_stat.nlinks);
printf("tamEnBytesLog=%d\n", p_stat.tamEnBytesLog);
printf("numBloquesOcupados=%d\n\n", p_stat.numBloquesOcupados);

bumount();

}
