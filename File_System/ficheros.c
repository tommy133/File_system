/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "ficheros.h"


int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes){
    int desp1;//deplazamiento
    int desp2;
    char buf_bloque[BLOCKSIZE];
    int bloque;//bloque fisico
    int escritos;
    int primerBLogico; //inicio donde escribimos
    int ultimoBLogico; //fin donde escribimos
    struct inodo inodo;

    mi_waitSem();
    if (leer_inodo(ninodo, &inodo)<0){
        fprintf(stderr, "mi_write_f()-->Error al leer el inodo");
        mi_signalSem();
        return -1;
    }

    if((inodo.permisos & 2)==2){

        primerBLogico=offset/BLOCKSIZE;
        ultimoBLogico=(offset+nbytes-1)/BLOCKSIZE;
        
        bloque=traducir_bloque_inodo(ninodo,primerBLogico,1);
        bread(bloque,buf_bloque);
        
        if(primerBLogico==ultimoBLogico){//escribimos en uno solo bloque 
            desp1=offset%BLOCKSIZE;
            bloque=traducir_bloque_inodo(ninodo,primerBLogico,1);
            bread(bloque,buf_bloque);
            memcpy(buf_bloque+desp1,buf_original,nbytes);
            bwrite(bloque,buf_bloque);
            escritos = nbytes;

        }else{//escribimos en varios bloques

            //primer bloque
            bloque=traducir_bloque_inodo(ninodo,primerBLogico,1);
            bread(bloque,buf_bloque);
            desp1 = offset % BLOCKSIZE; 
            memcpy(buf_bloque+desp1,buf_original,BLOCKSIZE-desp1);
            bwrite(bloque,buf_bloque);
            escritos = BLOCKSIZE-desp1;

            //intermedios
            for(int i =primerBLogico+1;i<ultimoBLogico;i++){
                bloque = traducir_bloque_inodo(ninodo, i, 1);
                bwrite(bloque,buf_original+(BLOCKSIZE-desp1)+(i-primerBLogico-1)*BLOCKSIZE);
                escritos += BLOCKSIZE;
            }

            //ultimo bloque
            bloque = traducir_bloque_inodo(ninodo, ultimoBLogico, 1);
            bread(bloque,buf_bloque);
            desp2=(offset+nbytes-1)%BLOCKSIZE;
            memcpy (buf_bloque, buf_original + (BLOCKSIZE - desp1) + (ultimoBLogico - primerBLogico - 1) * BLOCKSIZE, desp2 + 1);
            bwrite(bloque,buf_bloque);
            escritos += (desp2 + 1);//printf("escritos=%d\n",escritos);
        }
    }else{
        fprintf(stderr, "mi_write_f()-->No tiene permisos para escribir\n");
        mi_signalSem();
        return -1;
    }

    if (leer_inodo(ninodo, &inodo)<0){
        fprintf(stderr, "mi_write_f()-->Error al leer el inodo\n");
        mi_signalSem();
        return -1;
    }

    if ((escritos+offset)>inodo.tamEnBytesLog){
        inodo.tamEnBytesLog = escritos + offset;
        inodo.ctime=time(NULL);
    }

    inodo.mtime=time(NULL);
    escribir_inodo(ninodo, inodo);
    
    mi_signalSem();
    return escritos;
 }
 
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){
    int desp1;//deplazamiento
    int desp2;
    char buf_bloque[BLOCKSIZE];
    int bloque;//bloque fisico
    int primerBLogico; //inicio donde escribimos
    int ultimoBLogico; //fin donde escribimos
    int leidos;
    struct inodo inodo;

    mi_waitSem();
    leer_inodo(ninodo, &inodo);
    inodo.atime = time(NULL);
    escribir_inodo(ninodo, inodo);
    mi_signalSem();

    if (leer_inodo(ninodo, &inodo)<0){
        fprintf(stderr, "mi_read_f()-->Error al leer el inodo");
        return -1;
    }
    if((inodo.permisos & 4) == 4){
      if(offset >= inodo.tamEnBytesLog){
          leidos = 0 ;// No podemos leer nada
          return leidos;
      }
       if (offset + nbytes >= inodo.tamEnBytesLog){
          nbytes = inodo.tamEnBytesLog - offset;
          // leemos sólo los bytes desde el offset hasta EOF 
        }
        

        primerBLogico=offset/BLOCKSIZE;
        ultimoBLogico=(offset+nbytes-1)/BLOCKSIZE;
        
        desp1=offset%BLOCKSIZE;
        desp2=(offset+nbytes-1)%BLOCKSIZE;
        
        bloque = traducir_bloque_inodo(ninodo, primerBLogico,0);
        
        if (primerBLogico == ultimoBLogico){
            if (bloque != -1){
                bread(bloque, buf_bloque);
                memcpy(buf_original, buf_bloque+desp1, nbytes);
            }
            leidos = nbytes;
        }
        else {
            if (bloque != -1){
                bread(bloque, buf_bloque);
                memcpy(buf_original, buf_bloque+desp1, BLOCKSIZE-desp1);
            }
            leidos = BLOCKSIZE-desp1;
            for (int i=primerBLogico+1;i<ultimoBLogico;i++){
                bloque = traducir_bloque_inodo(ninodo,i,0);
                if (bloque!=-1){
                    bread(bloque, buf_original+(BLOCKSIZE-desp1)+(i-primerBLogico-1)*BLOCKSIZE);
                }
                leidos += BLOCKSIZE;
            }
            
            bloque = traducir_bloque_inodo(ninodo, ultimoBLogico,0);
            if (bloque!=-1){
                bread(bloque, buf_bloque);
                memcpy(buf_original+(nbytes-desp2-1), buf_bloque, desp2+1);
            }
            leidos += desp2 +1;
            
            localtime(&inodo.atime);
            escribir_inodo(ninodo, inodo);
        }
        
        }
        else {
            fprintf(stderr, "mi_read_f()-->No tiene permisos para leer");
    	    return -1;
        }
        return leidos;
     }

 int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){

    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo)<0){
    	fprintf(stderr, "mi_stat_f()-->Error al leer el inodo\n");
    	return -1;
    }

    p_stat->tipo=inodo.tipo;
    p_stat->permisos=inodo.permisos;     
    p_stat->atime=inodo.atime;
    p_stat->ctime=inodo.ctime;
    p_stat->mtime=inodo.mtime;
    p_stat->nlinks=inodo.nlinks;
    p_stat->tamEnBytesLog=inodo.tamEnBytesLog;
    p_stat->numBloquesOcupados=inodo.numBloquesOcupados;



    return 0;
 }
 
int mi_chmod_f(unsigned int ninodo, unsigned char permisos){

    struct inodo inodo;

    if( leer_inodo(ninodo,&inodo)!=-1){
        inodo.permisos=permisos;
        inodo.ctime = time(NULL);
    if(escribir_inodo(ninodo,inodo)!=-1){
        return 0;
    }
    else{
        fprintf(stderr, "mi_chmod_f()-->ERROR AL ESCRIBIR EL INODO");
        return -1;
    }
    }
    else{
        fprintf(stderr, "mi_chmod_f()-->ERROR AL LEER EL INODO");
        return -1;
    }

}
  
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){
    struct inodo inodo;
    unsigned int primerBL;

    if (leer_inodo(ninodo, &inodo)<0){
        fprintf(stderr, "mi_truncar_f()-->Error al leer el inodo");
        return -1;
    }
      
    if ((inodo.permisos & 2) != 2){
        fprintf(stderr, "mi_truncar_f() --> NO TIENE PERMISOS DE ESCRITURA");
        return -1;
    }
    if (nbytes % BLOCKSIZE == 0){
        primerBL = nbytes / BLOCKSIZE;
    } else {
        primerBL = nbytes / BLOCKSIZE +1;
    }

    int liberados=liberar_bloques_inodo(primerBL, inodo);

    if (leer_inodo(ninodo, &inodo)<0){
        fprintf(stderr, "mi_truncar_f()-->Error al leer el inodo");
        return -1;
    }
      inodo.mtime = time(NULL);
      inodo.ctime = time(NULL);
      inodo.tamEnBytesLog = nbytes;
      inodo.numBloquesOcupados -= liberados;
      escribir_inodo(ninodo, inodo);
    
    return liberados;
}
