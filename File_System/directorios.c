/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/

#include "directorios.h"

struct UltimaEntrada UltimaEntradaEscritura;
struct UltimaEntrada UltimaEntradaLectura;

//Obtiene primer directorio o fichero del path
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    const char bar = '/';
    char *dir;
    if (camino[0] != bar){
        return ERROR_CAMINO_INCORRECTO; //El primer carácter no es el separador '/' o camino vacío
    }

    camino++;
    dir = strchr(camino, bar);
    if (dir != NULL){
        strncpy(inicial,camino,(dir-camino));
        camino = dir; 
        strcpy(final,camino);
        *tipo = 'd';
        return 'd';
    }
    else{
        strcpy(inicial,camino);
        strcpy(final, "");
        *tipo = 'f';
        return 'f';
    }

}
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos){
    if (strcmp(camino_parcial,"/")== 0){  //camino parcial es "/"
        *p_inodo = 0; //Raiz asociada siempre al inodo 0
        *p_entrada = 0;
        return 0;
    }
    struct entrada entrada;memset(&entrada,0,sizeof(entrada));
    char inicial[sizeof(entrada.nombre)];memset(inicial,0,sizeof(entrada.nombre));
    char final[strlen(camino_parcial)];memset(final,0,strlen(camino_parcial));
    char tipo;

    if (extraer_camino(camino_parcial,inicial,final,&tipo) == -1) {//Si la extraccion de camino falla, abortamos la busqueda 
        return ERROR_CAMINO_INCORRECTO;
    }
    //fprintf(stderr, "buscar_entrada()-->inicial: %s, final: %s, reservar: %d\n", inicial, final, reservar);

    struct inodo inodo_dir;
    leer_inodo(*p_inodo_dir, &inodo_dir);

    if ((inodo_dir.permisos & 4) != 4){
        return ERROR_PERMISO_LECTURA;
    }

    //unsigned int ent_per_bloq = BLOCKSIZE / sizeof(struct entrada);
    //struct entrada buffer[ent_per_bloq];

    //Calcular cantidad de entradas que contiene el inodo
    unsigned int n_entradas = inodo_dir.tamEnBytesLog/sizeof(struct entrada);
    unsigned int i_entrada_inodo = 0; //nº de entrada inicial

    if (n_entradas > 0){
        memset(&entrada,0,sizeof(entrada));
        mi_read_f(*p_inodo_dir, &entrada, 0, sizeof(struct entrada));

        while ((i_entrada_inodo < n_entradas) && (strcmp(inicial, entrada.nombre))!=0){
            i_entrada_inodo++;
            memset(&entrada,0,sizeof(entrada));
            mi_read_f(*p_inodo_dir, &entrada, i_entrada_inodo*sizeof(struct entrada), sizeof(struct entrada));//...leer la entrada
        }
    }
    
    if ( (i_entrada_inodo == n_entradas) && (strcmp(inicial, entrada.nombre)!=0) ){//La entrada no existe
        switch (reservar){
            case 0: //modo consulta. Como no existe retornamos error
                return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
                break;
            case 1: //Modo escritura
                //Creamos la entrada en el directorio referenciado por *p_inodo_dir
                //si es fichero no permitir escritura
                if (inodo_dir.tipo == 'f'){
                    return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
                }
                //si es directorio comprobar que tiene permiso de escritura
                if ((inodo_dir.permisos & 2) != 2){
                    return ERROR_PERMISO_ESCRITURA;
                } else {
                    strcpy(entrada.nombre, inicial);
                    if (tipo == 'd'){
                        if (strcmp(final, "/") == 0){               
                            entrada.ninodo = reservar_inodo('d', permisos);
                        } else{ //cuelgan más diretorios o ficheros
                            return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                        }
                    } else{
                        entrada.ninodo = reservar_inodo('f', permisos);
                    }
                    
                    //fprintf(stderr, "buscar_entrada()-->reservado inodo %d tipo %c con permisos %d para %s\n", entrada.ninodo, tipo, permisos, entrada.nombre);
                    //fprintf(stderr, "buscar_entrada()-->creada entrada: %s, %d\n", entrada.nombre, entrada.ninodo);

                    //escribir entrada
                    if (mi_write_f(*p_inodo_dir, &entrada, i_entrada_inodo*sizeof(struct entrada), sizeof(struct entrada)) == -1) { // Error de escritura
                        if (entrada.ninodo != -1){
                            liberar_inodo(entrada.ninodo);
                        }
                        return EXIT_FAILURE;
                    }
                }
                break;
        }
    }
    if(strcmp(final, "/") == 0 || strcmp(final, "") == 0){
        if((i_entrada_inodo < n_entradas) && (reservar == 1)){
            *p_entrada = i_entrada_inodo;
            return ERROR_ENTRADA_YA_EXISTENTE;//modo escritura y la entrada ya existe
        } // cortamos la recursividad

        *p_inodo = entrada.ninodo;
        *p_entrada = i_entrada_inodo;
        return EXIT_SUCCESS;
    }else{
        *p_inodo_dir = entrada.ninodo;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);

    }    
}

void mostrar_error_buscar_entrada(int error) {
   // fprintf(stderr, "Error: %d\n", error);
   switch (error) {
    case -1: fprintf(stderr, "buscar_entrada()--> Error: Camino incorrecto.\n"); break;
    case -2: fprintf(stderr, "buscar_entrada()--> Error: Permiso denegado de lectura.\n"); break;
    case -3: fprintf(stderr, "buscar_entrada()--> Error: No existe el archivo o el directorio.\n"); break;
    case -4: fprintf(stderr, "buscar_entrada()--> Error: No existe algún directorio intermedio.\n"); break;
    case -5: fprintf(stderr, "buscar_entrada()--> Error: Permiso denegado de escritura.\n"); break;
    case -6: fprintf(stderr, "buscar_entrada()--> Error: El archivo ya existe.\n"); break;
    case -7: fprintf(stderr, "buscar_entrada()--> Error: No es un directorio.\n"); break; 
   }
}

int mi_creat(const char *camino, unsigned char permisos){
    unsigned int p_inodo_dir = 0, p_inodo, p_entrada;
    int res;

    mi_waitSem();
    if ( (res = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos)) < 0){
        mostrar_error_buscar_entrada(res);
        mi_signalSem();
        return res;;
    }
    mi_signalSem();
    return EXIT_SUCCESS;
}

int mi_dir(const char *camino, char *buffer){

    unsigned int p_inodo_dir = 0, p_inodo, p_entrada;
    struct inodo inodo;
      
    if (camino[strlen(camino)-1] != '/') { //Llevar si fas sa millora per a fitxers
        fprintf(stderr, "mi_dir()--> Error. El camino no se corresponde con un directorio.\n"); 
        return -1;
    } 
    int res;   
    if ( (res = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 7)) < 0){
        mostrar_error_buscar_entrada(res);
        return res;;
    } 
    leer_inodo(p_inodo, &inodo);
    if (inodo.tipo == 'd') {
	
        if ((inodo.permisos & 4) == 4){
            int n_entradas = inodo.tamEnBytesLog/sizeof(struct entrada);
            struct entrada entrada;
            struct inodo e_inodo;
            for (int i = 0; i < n_entradas; i++) {
                mi_read_f(p_inodo,&entrada,i*sizeof(struct entrada),sizeof(struct entrada));
                leer_inodo(entrada.ninodo, &e_inodo);
                
                char tipo = e_inodo.tipo;
                char cToStr[2];
                cToStr[1]='\0';
                cToStr[0]=tipo;
                strcat(buffer, cToStr);

                strcat(buffer, "\t\t");

                switch (e_inodo.permisos){
                    case 2: strcat(buffer, "-w-");break;
                    case 4: strcat(buffer, "r--");break;
                    case 6: strcat(buffer, "rw-");break;
                    case 7: strcat(buffer, "rwx");break;
                }

                strcat(buffer, "\t\t");

                struct tm *tm; //ver info: struct tm
                char tmp[100];
                tm = localtime(&e_inodo.mtime);
                sprintf(tmp,"%d-%02d-%02d %02d:%02d:%02d\t",tm->tm_year+1900,
                    tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
                    strcat(buffer,tmp);
                char t[256];
                sprintf(t,"%d",e_inodo.tamEnBytesLog);
                strcat(buffer, t);
                strcat(buffer, "\t\t");
                
                strcat(buffer,entrada.nombre);
                strcat(buffer,"|");
            }
            return n_entradas;
        } else {
            printf("mi_dir()--> Error: El directorio no tiene permisos de lectura.\n");
            return -1;
        }
    } else {
        printf("mi_dir()--> Error: El camino no es a ningun directorio.\n");
        return -1;
    }
}

int mi_chmod(const char *camino, unsigned char permisos){
    unsigned int p_inodo_dir = 0, p_inodo, p_entrada;

    int res;
    
    if ( (res = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, permisos)) < 0){
        mostrar_error_buscar_entrada(res);
        return res;;
    } 

    if (mi_chmod_f(p_inodo, permisos) < 0){
        fprintf(stderr, "mi_chmod()--> Error al cambiar los permisos del fichero o directorio");
        return -1;
    }

    return EXIT_SUCCESS;
}

int mi_stat(const char *camino, struct STAT *p_stat){
    unsigned int p_inodo_dir = 0, p_inodo, p_entrada;

    int res;
    
    if ( (res = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 7)) < 0){
        mostrar_error_buscar_entrada(res);
        return res;;
    } 

    if (mi_stat_f(p_inodo, p_stat) < 0){
        fprintf(stderr, "mi_stat()--> Error al mostrar el nº de inodo");
        return -1;
    }

    return p_inodo;
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
    unsigned int p_inodo_dir = 0, p_inodo, p_entrada;
    struct inodo inodo;

    if (strcmp (camino, UltimaEntradaEscritura.camino) == 0){
        p_inodo = UltimaEntradaEscritura.p_inodo;
    } else{
        int res;
    
        if ( (res = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 7)) < 0){
            mostrar_error_buscar_entrada(res);
            return res;;
        }

        strcpy(UltimaEntradaEscritura.camino, camino);
        UltimaEntradaEscritura.p_inodo = p_inodo;
    }
    
    leer_inodo(p_inodo, &inodo);

    if (inodo.tipo == 'd'){
        fprintf(stderr, "mi_write()--> El camino no es ningún fichero\n");
        return -1;
    }

    int escritos;
    if ((escritos = mi_write_f(p_inodo, buf, offset, nbytes)) < 0) {
        fprintf(stderr, "mi_write()--> Error al escribir en el fichero\n");
        return -1;
    }

    return escritos;    
}

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){
    unsigned int p_inodo_dir = 0, p_inodo, p_entrada;
    struct inodo inodo;

    if (strcmp (camino, UltimaEntradaLectura.camino) == 0){
        p_inodo = UltimaEntradaLectura.p_inodo;
    } else{
        int res;
    
        if ( (res = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 7)) < 0){
            mostrar_error_buscar_entrada(res);
            return res;;
        }

        strcpy(UltimaEntradaLectura.camino, camino);
        UltimaEntradaLectura.p_inodo = p_inodo;
    }
    
    leer_inodo(p_inodo, &inodo);

    

    int leidos;
    if ((leidos = mi_read_f(p_inodo, buf, offset, nbytes)) < 0) {
        fprintf(stderr, "mi_read()--> Error al leer en el fichero");
        return -1;
    }

    return leidos;   

}

int mi_link(const char *camino1, const char *camino2){
    unsigned int p_inodo_dir1 = 0, p_inodo1, p_entrada1;
    unsigned int p_inodo_dir2 = 0, p_inodo2, p_entrada2;

    //Comprobamos que camino1 y camino2 se refieran a un fichero
    if (camino1[strlen(camino1)-1] == '/') { 
        fprintf(stderr, "mi_link()-->Error. El camino1 ha de referirse a un fichero.\n"); 
        return -1;
    } 

    if (camino2[strlen(camino2)-1] == '/') { 
        fprintf(stderr, "mi_link()-->Error. El camino2 ha de referirse a un fichero.\n"); 
        return -1;
    }

    int res;
    mi_waitSem();
    if ( (res = buscar_entrada(camino1, &p_inodo_dir1, &p_inodo1, &p_entrada1, 0, 7)) < 0){
        mostrar_error_buscar_entrada(res);
        mi_signalSem();
        return res;
    }

    if ( (res = buscar_entrada(camino2, &p_inodo_dir2, &p_inodo2, &p_entrada2, 1, 6)) < 0){
        mostrar_error_buscar_entrada(res);
        mi_signalSem();
        return res;
    }

    struct entrada entrada;
    mi_read_f(p_inodo_dir2,&entrada,p_entrada2*sizeof(struct entrada),sizeof(struct entrada)); //Leemos la entrada correspondiente a camino 2
    entrada.ninodo = p_inodo1; //Creamos el enlace
    mi_write_f(p_inodo_dir2,&entrada,p_entrada2*sizeof(struct entrada),sizeof(struct entrada)); //Escribimos la entrada modificada

    liberar_inodo(p_inodo2); //Liberamos el inodo que se ha asociado a la entrada creada

    struct inodo inodo1;
    leer_inodo(p_inodo1, &inodo1);
    inodo1.nlinks++;
    inodo1.ctime = time(NULL);
    escribir_inodo(p_inodo1, inodo1);

    mi_signalSem();
    return 0;
}

int mi_unlink(const char *camino){
    unsigned int p_inodo_dir = 0, p_inodo, p_entrada;

    int res;
    mi_waitSem();
    if ( (res = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 7)) < 0){
        mostrar_error_buscar_entrada(res);
        mi_signalSem();
        return res;
    }

    struct inodo inodo;
    leer_inodo(p_inodo, &inodo);

    if (inodo.tipo == 'd' && inodo.tamEnBytesLog > 0){
        fprintf(stderr, "mi_unlink()--> Error: No se puede borrar un directorio no vacío!\n");
        mi_signalSem();
        return -1;
    }

    struct inodo inodo_dir;
    leer_inodo(p_inodo_dir, &inodo_dir);
    int n_entradas = inodo_dir.tamEnBytesLog/sizeof(struct entrada);

    if (p_entrada != n_entradas-1){
        struct entrada entrada;
        mi_read_f(p_inodo_dir,&entrada,inodo_dir.tamEnBytesLog - sizeof(struct entrada),sizeof(struct entrada));
        mi_write_f(p_inodo_dir,&entrada,p_entrada*sizeof(struct entrada),sizeof(struct entrada));
    }
        
    mi_truncar_f(p_inodo_dir, inodo_dir.tamEnBytesLog - sizeof(struct entrada));

    inodo.nlinks--;
    if (inodo.nlinks == 0) {
        liberar_inodo(p_inodo);
    } else {
        inodo.ctime = time(NULL);
        escribir_inodo(p_inodo, inodo);
    }

    mi_signalSem();
    return 0;
}

