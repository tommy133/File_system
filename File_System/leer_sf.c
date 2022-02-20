/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "directorios.h"


int main(int argc,char**argv){

    struct superbloque sb;/*
    struct inodo inodo;
    int ninodo=1;
    char reservar=1;
    int b_logicos_trad[] = {8,204,30004,400004,16843019};
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];*/
    
    //Montamos el dispositivo.
    bmount(argv[1]);
    
    
   
    if(bread(posSB,&sb)<0)return -1;
    printf("\n");
    printf("DATOS DEL SUPERBLOQUE\n\n");
    printf("posPrimerBloqueMB = %i\n",sb.posPrimerBloqueMB);
    printf("posUltimoBloqueMB =  %i\n",sb.posUltimoBloqueMB);
    printf("posPrimerBloqueAI =  %i\n",sb.posPrimerBloqueAI);	
    printf("posUltimoBloqueAI = %i\n",sb.posUltimoBloqueAI);	
    printf("posPrimerBloqueDatos = %i\n",sb.posPrimerBloqueDatos);	
    printf("posUltimoBloqueDatos =  %i\n",sb.posUltimoBloqueDatos);
    printf("posInodoRaiz = %i\n",sb.posInodoRaiz);	
    printf("posPrimerInodoLibre = %i\n",sb.posPrimerInodoLibre);
    printf("cantBloquesLibres = %i\n",sb.cantBloquesLibres);
    printf("cantInodosLibres = %i\n",sb.cantInodosLibres);	
    printf("totBloques = %i\n",sb.totBloques);
    printf("totInodos = %i\n",sb.totInodos);
    printf("\n");
    /*
    printf ("sizeof struct superbloque is: %lu\n", sizeof(struct superbloque));
    printf ("sizeof struct inodo is: %lu\n", sizeof(struct inodo));
    printf("\n");
    */
    /*
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    
    
    printf("RECORRIDO DE LA LISTA ENLAZADA DE INODOS LIBRES: \n");
    
    for (int i= sb.posPrimerBloqueAI; i<=sb.posUltimoBloqueAI;i++){
        bread(i, &inodos);
        for (int j=0; j<BLOCKSIZE / INODOSIZE; j++){
            printf("%d\n", inodos[j].punterosDirectos[0]);
        }
        
    }
    */

    /*
    int bloque_reserva = reservar_bloque();
    printf("Se ha reservado el bloque físico nº %d que era el primero libre indicado por el mapa de bits\n", bloque_reserva);
    if(bread(posSB,&sb)<0)return -1;
    printf("sb.cantBloquesLibres = %d\n", sb.cantBloquesLibres);

    liberar_bloque(bloque_reserva);
    if(bread(posSB,&sb)<0)return -1;
    printf("Liberamos ese bloque y después sb.cantBloquesLibres = %d\n", sb.cantBloquesLibres);

    printf("\nMAPA DE BITS CON BLOQUES DE METADATOS OCUPADOS\n");

    printf("leer_bit(0) = %u\n", leer_bit(0) );
    printf("leer_bit(%d) = %u\n", sb.posPrimerBloqueMB, leer_bit(sb.posPrimerBloqueMB));
    printf("leer_bit(%d) = %u\n", sb.posUltimoBloqueMB, leer_bit(sb.posUltimoBloqueMB));
    printf("leer_bit(%d) = %u\n", sb.posPrimerBloqueAI, leer_bit(sb.posPrimerBloqueAI));
    printf("leer_bit(%d) = %u\n", sb.posUltimoBloqueAI, leer_bit(sb.posUltimoBloqueAI));
    printf("leer_bit(%d) = %u\n", sb.posPrimerBloqueDatos, leer_bit(sb.posPrimerBloqueDatos));
    printf("leer_bit(%d) = %u\n", sb.posUltimoBloqueDatos, leer_bit(sb.posUltimoBloqueDatos));

    printf("\nDATOS DEL DIRECTORIO RAIZ\n");
    leer_inodo(ninodo, &inodo);

    printf("DATOS DEL INODO RESERVADO %d\n", ninodo);
    printf("tipo: %c\n", inodo.tipo);
    printf("permisos: %d\n", inodo.permisos);
    printf("nlinks: %d\n", inodo.nlinks);
    printf("tamEnBytesLog: %d\n",inodo.tamEnBytesLog);
    printf("numBloquesOcupados: %d\n", inodo.numBloquesOcupados);
    ts = localtime(&inodo.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    printf("ID: %d ATIME: %s\n MTIME: %s\n CTIME: %s\n",ninodo,atime,mtime,ctime);
    printf("\n");
    */
    /*
    printf("INODO 1. TRADUCCION DE LOS BLOQUES LOGICOS 8, 204, 30004, 400004, 16843019\n");
    reservar_inodo('d',7);
    
    for (int i=0; i<5; i++){
        traducir_bloque_inodo(ninodo, b_logicos_trad[i], reservar);
    }
    
    leer_inodo(ninodo, &inodo);
    printf("DATOS DEL INODO RESERVADO %d\n", ninodo);
    printf("tipo: %c\n", inodo.tipo);
    printf("permisos: %d\n", inodo.permisos);
    printf("nlinks: %d\n", inodo.nlinks);
    printf("tamEnBytesLog: %d\n",inodo.tamEnBytesLog);
    printf("numBloquesOcupados: %d\n", inodo.numBloquesOcupados);
    ts = localtime(&inodo.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    printf("ID: %d ATIME: %s\n MTIME: %s\n CTIME: %s\n",ninodo,atime,mtime,ctime);
    printf("\n"); */
}
