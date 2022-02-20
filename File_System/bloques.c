/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "bloques.h"
#include "semaforo_mutex_posix.h"

static int descriptor = 0;
static sem_t *mutex;
static unsigned int inside_sc = 0;

/*
 *bmount( ): Monta la imagen del disco
 *camino: puntero a la ruta donde se encuentra la imagen del disco
 *return: -1 si ha habido error o el descriptor si ha ido bien
 */

 int bmount(const char *camino){  
    umask(000);
    if (descriptor > 0) {
       close(descriptor);
   }
   if ((descriptor = open(camino, O_RDWR | O_CREAT, 0666)) == -1) {
      fprintf(stderr, "Error: bloques.c → bmount() → open()\n");
   }
   if (!mutex) { //mutex == 0
   //el semáforo es único y sólo se ha de inicializar una vez en nuestro sistema (lo hace el padre)
    mutex = initSem(); //lo inicializa a 1
    if (mutex == SEM_FAILED) {
        return -1;
    }
   }

    return descriptor;
}

/*
 *bumount(): llama a close() para liberar el descriptor de ficheros
 *return: 0 si se ha cerrado el fichero correctamente y -1 en caso contrario
 */

 int bumount(){

    descriptor = close(descriptor); 
   // hay que asignar el resultado de la operación a la variable ya que bmount() la utiliza
   if (descriptor == -1) {
       fprintf(stderr, "Error: bloques.c → bumount() → close(): %d: %s\n", errno, strerror(errno));
       return -1;
   }
   deleteSem(); // borramos semaforo 
   return 0;

}

/*
 *bwrite( ):Escribe el contenido del buffer en el bloque especificado
 *nbloque: número del bloque en el cual queremos escribir
 *buf: buffer de memoria del tamaño de un bloque que contiene
 *lo que queremos escribir
 *return: número de bytes que ha podido escribir o -1 si hay error
 */
int bwrite(unsigned int nbloque, const void *buf){
	off_t offset=nbloque*BLOCKSIZE;

    if(lseek(descriptor,offset,SEEK_SET) < 0){
        perror("bwrite()-->ERROR AL ESCRIBIR EL FICHERO");
        return -1;
    }
    return write(descriptor, buf,BLOCKSIZE);
}

/*
 *bread( ): operación inversa a bwrite( )
 *return: número de bytes que ha podido escribir o -1 si hay error
 */
 int bread(unsigned int nbloque, void *buf){
 	size_t b;
    lseek(descriptor,nbloque*BLOCKSIZE,SEEK_SET);
    b= read(descriptor,buf,BLOCKSIZE);

    if(b==-1){
        perror("bread()-->ERROR AL LEER EL FICHERO");
        return -1;
    }
    else{
        return b;
    }
   
}

void mi_waitSem(){
    if (!inside_sc) {
    waitSem(mutex);
    }
    inside_sc++;
}

void mi_signalSem() {
    inside_sc--;
    if (!inside_sc) {
        signalSem(mutex);
    }
}



