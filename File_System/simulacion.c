/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "simulacion.h"


int acabados=0;
void reaper(){
  pid_t ended;
  signal(SIGCHLD, reaper);
  while ((ended=waitpid(-1, NULL, WNOHANG))>0) {
    acabados++;
   //Podemos testear qué procesos van acabando:
    //fprintf(stderr, "acabado: %d total acabados: %d\n", ended, acabados);
  }
}

int main(int argc, char **argv) {  
    char *disco = argv[1];
    char nombre_dir[LONG_ENTRADA];
    char nombre_pid[LONG_ENTRADA+100];
    char nombre_fichero[LONG_ENTRADA+200];
	if (argc != 2) {
        fprintf(stderr, "Error. Sintaxis correcta: ./simulacion <disco>\n");
        return -1;
    }
    bmount(disco);
    struct tm *tm;
    time_t tiempo = time(NULL);
    tm = localtime(&tiempo);
    sprintf(nombre_dir,"/simul_%d%02d%02d%02d%02d%02d/",tm->tm_year+1900, tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
    //Creamos el directorio
    if (mi_creat(nombre_dir, 7) < 0){
        fprintf(stderr, "Error al crear el directorio de simulación");
          if(bumount()==-1){
            fprintf(stderr, "Error al cerrar el fichero");
            return -1;
          }
        return -1;
    }
    signal(SIGCHLD, reaper);
    pid_t pid;
    for (int i=1;i<=NUMPROC;i++){
      pid=fork();
        int num_pid = getpid();
        if(pid<0){
            fprintf(stderr, "Error al crear el proceso ");
            return -1;
        }       
        if(pid==0){
            if(bmount(disco)==-1){
              fprintf(stderr, "Error al montar el disco ");
              return -1;              
            }
            sprintf(nombre_pid,"%sproceso_%d/",nombre_dir,num_pid);
            if(mi_creat(nombre_pid,7)<0){
                fprintf(stderr, "Error al crear el directorio de simulación");
                if(bumount()==-1){
                  fprintf(stderr, "Error al cerrar el fichero");
                  return -1;
                }
                return -1;
            }
            //Creamos el fichero
            sprintf(nombre_fichero,"%sprueba.dat",nombre_pid);
            if(mi_creat(nombre_fichero,7)<0){
              fprintf(stderr, "Error al crear el directorio de simulación");
              if(bumount()==-1){
                  fprintf(stderr, "Error al cerrar el fichero");
                  return -1;
                }
                return -1;
            }
            srand(time(NULL)+getpid());
            struct REGISTRO registro;
            int j;
            for(j=0;j<ESCRITURAS;j++){
                registro.fecha=time(NULL);
                registro.pid=getpid();
                registro.nEscritura=j+1;
                registro.nRegistro=rand()%REGMAX;
                if(mi_write(nombre_fichero,&registro,registro.nRegistro *sizeof(struct REGISTRO),sizeof(struct REGISTRO))==-1){
                  fprintf(stderr, "Error al crear el directorio de simulación");
                  if(bumount()==-1){
                    fprintf(stderr, "Error al cerrar el fichero");
                    return -1;
                  } 
                  return -1;
                }
                //printf("Escritura %d en %s\n", j+1, nombre_fichero);
                usleep(50000);
            }
            //fprintf(stderr,"Proceso %d: Completadas %d escrituras en %s\n",i,j,nombre_fichero);
            if(bumount()==-1){
              fprintf(stderr, "Error al cerrar el fichero");
              return -1;
            } 
            exit(0);
        }
        usleep(200000);
    }
    while(acabados<NUMPROC){
        pause();
    }
    if(bumount()==-1){
        fprintf(stderr, "Error al cerrar el fichero");
        return -1;
    }
    printf("%s\n", nombre_dir); 
    exit(0);

}
