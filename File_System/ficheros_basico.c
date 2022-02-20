/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "ficheros_basico.h"


int tamMB (unsigned int nbloques){
    int heuristic=nbloques/8;
    if((heuristic%BLOCKSIZE) != 0 ){ //Mirar si hay que añadir un bloque extra
        return heuristic/BLOCKSIZE+1;
    }else{  
        return (heuristic/BLOCKSIZE);
    }

}

int tamAI (unsigned int ninodos){ //ninodos lo pasa mi_mkfs.c
    int bytes_inodo = ninodos*INODOSIZE;
    if (bytes_inodo% BLOCKSIZE != 0){ // INODOSIZE define nuevo en bloques.h
        return (bytes_inodo/BLOCKSIZE)+ 1;
    }
    return bytes_inodo/BLOCKSIZE;
}   

int initSB(unsigned int nbloques, unsigned int ninodos){
    
    struct superbloque sb;
    
    sb.posPrimerBloqueMB = posSB + tamSB;
    sb.posUltimoBloqueMB = sb.posPrimerBloqueMB+tamMB(nbloques)-1;
    sb.posPrimerBloqueAI = sb.posUltimoBloqueMB+1;
    sb.posUltimoBloqueAI = sb.posPrimerBloqueAI+tamAI(ninodos)-1;
    sb.posPrimerBloqueDatos = sb.posUltimoBloqueAI+1;
    sb.posUltimoBloqueDatos = nbloques-1;
    sb.posInodoRaiz = 0;
    sb.posPrimerInodoLibre = 0;
    sb.cantBloquesLibres = nbloques;
    sb.cantInodosLibres = ninodos;
    sb.totBloques = nbloques;
    sb.totInodos = ninodos; 
    
    if(bwrite(posSB, &sb)==-1){ //Escribe la estructura en el bloque posSB
        fprintf(stderr, "initSB()-->Error de escritura en el superbloque\n");
        return -1;
    }
    printf("Se ha escrito el superbloque correctamente\n");
    return 0;
}

int initMB(){
    
    struct superbloque sb;
    unsigned char buff[BLOCKSIZE];
    
    if (bread(posSB, &sb)==-1){
        fprintf(stderr, "initMB()-->Error de lectura del superbloque\n");
        return -1;
    }
    
    memset(buff,0,BLOCKSIZE); //ponemos a 0 todos los bits del buffer

    for(int i=sb.posPrimerBloqueMB;i<=sb.posUltimoBloqueMB+1;i++){
        if(bwrite(i,buff)==-1){
            fprintf(stderr, "initMB()-->Error de escritura en el mapa de bits");
            return -1;
        }
       
    }

    for (int i=posSB; i<=sb.posUltimoBloqueAI; i++){
        escribir_bit(i,1);
        sb.cantBloquesLibres--;
    }

    if(bwrite(posSB, &sb) == -1){
        
        fprintf(stderr, "initMB()-->ERROR, no se ha podido escribir el superbloque \n");
        
        return -1;
    }
    
    
    return 0;
}

int initAI(){
    struct superbloque sb;
    int inodosBloque = BLOCKSIZE / INODOSIZE;
    struct inodo inodos [ inodosBloque ];
    int contInodos;
    
    
    if (bread(posSB, &sb)==-1){
        fprintf(stderr, "initAI()-->Error de lectura del superbloque\n");
        return -1;
    }
    
    contInodos = sb.posPrimerInodoLibre + 1; //si hemos inicializado sb.posPrimerInodoLibre = 0
    
    for (int i= sb.posPrimerBloqueAI; i<=sb.posUltimoBloqueAI;i++) {
        for (int j=0; j<inodosBloque; j++) {
            inodos[j].tipo = 'l'; //Inodo libre
            if (contInodos < sb.totInodos) {
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }
            else { //Hemos llegado al último inodo
               inodos[j].punterosDirectos[0] = UINT_MAX;
            }
        }
        
        if (bwrite(i, inodos)<0){
            fprintf(stderr, "initAI()-->Error de escritura en el bloque de inodos\n");
            return -1;
        }
        
    }
    
    if(bwrite(posSB, &sb) == -1){
        
        fprintf(stderr, "initAI()-->Error de escritura en el superbloque");
        
        return -1;
    }   
    
    return 0;
}  

/*
 *escribir_bit( ): escribe 1 o 0 en un determinado bit del MB
 *nbloques: número de bloque físico
 *bit: valor del bit específico a escribir en el MB 
 *return: 0 si todo ha ido bien, -1 en caso contrario 
 */

int escribir_bit(unsigned int nbloque, unsigned int bit){
    struct superbloque sb;
    int posbyte; 
    int posbit;  
    int nbloqueMB; 
    int nbloqueabs;
    unsigned char bufferMB[BLOCKSIZE];
    unsigned char mascara = 128; // MAsCARA PARA CAMBIAR EL BIT
    
    if (bread(posSB, &sb)<0){
        fprintf(stderr, "escribir_bit()-->Error de lectura del superbloque\n");
        return -1;
    }
    
    posbyte = nbloque / 8;
    posbit = nbloque % 8;
    //printf("%d",posbit);
    nbloqueMB = posbyte/BLOCKSIZE;
    nbloqueabs = nbloqueMB + sb.posPrimerBloqueMB;
    
    if (bread(nbloqueabs,bufferMB)<0){
        fprintf(stderr, "escribir_bit()-->Error de lectura del mapa de bits\n");
        return -1;
    }
    
    posbyte = posbyte % BLOCKSIZE;
    mascara >>= posbit; // desplazamiento de bits a la derecha

    if (bit==1){
        bufferMB[posbyte] |= mascara; // |= HACE UNA OR BIT A BIT  PARA PONER EL BIT A 1
        }
    else if(bit==0){
        bufferMB[posbyte] &= ~mascara; // &= HACE UNA AND BIT A BIT PARA PONER EL BIT A 0
        }
    if (bwrite(nbloqueabs,bufferMB)<0){
        fprintf(stderr, "escribir_bit()-->Error de escritura en el mapa de bits");
        return -1;
    }
        
    return 0;
}

unsigned char leer_bit(unsigned int nbloque){
    struct superbloque sb;

    //SIGUE EL MISMO MÉTODO PARA CONSEGUIR EL BIT QUE LA FUNCION ANTERIOR

    int posbyte; //POSICION DEL BYTE
    int posbit;  //POSICION DEL BIT
    int nbloqueMB; //EN QUE BLOQE ESTA
    int nbloqueabs;
    unsigned char bufferMB[BLOCKSIZE];
    unsigned char mascara = 128; // MACARA PATRA CAMBIAR EL BIT
    
    if (bread(posSB, &sb)<0){
        fprintf(stderr, "leer_bit()-->Error de lectura del superbloque\n");
        return -1;
    }
    
    posbyte = nbloque / 8;
    posbit = nbloque % 8;
    nbloqueMB = posbyte/BLOCKSIZE;
    nbloqueabs = nbloqueMB + sb.posPrimerBloqueMB;
    
    if (bread(nbloqueabs,bufferMB)<0){
        fprintf(stderr, "leer_bit()-->Error de lectura del mapa de bits\n");
        return -1;
    }
    posbyte = posbyte % BLOCKSIZE;
    mascara >>= posbit;
    mascara &= bufferMB[posbyte]; 

    return mascara >>= (7-posbit); 
}

int reservar_bloque(){

    struct superbloque sb;
    unsigned char bufferMB[BLOCKSIZE];
    unsigned char bufferAux[BLOCKSIZE];
    unsigned int posbit;
    unsigned int nbloque;
    unsigned char mascara = 128;//100000000

    if(bread(posSB, &sb) <0){
        fprintf(stderr, "reservar_bloque()-->Error de lectura del superbloque");
        return -1;
    }
    
    if(sb.cantBloquesLibres == 0){      
        fprintf(stderr, "reservar_bloque()-->Error, no quedan bloques para reservar");
        return -1;
    }
    memset(bufferAux,255,BLOCKSIZE); //inicializamos a 1 el buffer auxiliar

    for(int posBloqueMB=sb.posPrimerBloqueMB; posBloqueMB <= sb.posUltimoBloqueMB; posBloqueMB++){
        if(bread(posBloqueMB,bufferMB) < 0){
            fprintf(stderr, "reservar_bloque()-->Error de lectura del mapa de bits");            
            return -1;
        }
        
        if(memcmp(bufferMB,bufferAux,BLOCKSIZE) < 0){//Buscamos un 0 en el bloque

            for (int posbyte=0; posbyte<BLOCKSIZE; posbyte++){ //Recorremos los bloques del mapa de bits

                if (bufferMB[posbyte] <255) {
                    posbit = 0;
                    while (bufferMB[posbyte] & mascara) {
                        posbit++;
                        bufferMB[posbyte] <<= 1;//desplaz.bits izqd
                    }

                    nbloque = ((posBloqueMB - sb.posPrimerBloqueMB) * BLOCKSIZE + posbyte)* 8 + posbit;
                    
                    if(escribir_bit(nbloque,1)<0){
                        fprintf(stderr, "reservar_bloque()-->Error de reserva del bloque");
                        return -1;
                    }
                    sb.cantBloquesLibres--;
                    if(bwrite(posSB,&sb)<0){
                        fprintf(stderr, "reservar_bloque()-->Error actualizando el superbloque");
                        return -1;
                    }                   
                    return nbloque; 
                }
            }
            
        }

    }

    return -1;
}

int liberar_bloque(unsigned int nbloque){

     struct superbloque sb;

     if (bread(posSB, &sb)<0){
         fprintf(stderr, "liberar_bloque()-->Error de lectura del superbloque");
         return -1;
     }

     escribir_bit(nbloque,0); // ponemos a cero el bit que queremos liberar
     sb.cantBloquesLibres++;  // incrementamos en 1 la catidad de bloques liberes que en el superbloque
    
     if (bwrite(posSB, &sb)<0){ //Salvamos el superbloque
         fprintf(stderr, "liberar_bloque()-->Error  de escritura del superbloque");
         return -1;
     } 

     return nbloque;

 }

 int escribir_inodo(unsigned int ninodo, struct inodo inodo){

     struct superbloque sb;
     int bloqueAI;
     struct inodo inodos[BLOCKSIZE/INODOSIZE];

    if (bread(posSB, &sb)==-1){ //leemos el superbloque y encontramos el AI
        fprintf(stderr, "escribir_inodo()-->Error de lectura del superbloque\n");
        return -1;
        }
    bloqueAI=(ninodo/(BLOCKSIZE/INODOSIZE))+sb.posPrimerBloqueAI;
    if (bread(bloqueAI, inodos)<0){
        fprintf(stderr, "escribir_inodo()-->Error de lectura del bloque");
         return -1;
     }
    inodos[ninodo%(BLOCKSIZE/INODOSIZE)]=inodo;
    if (bwrite(bloqueAI,inodos)<0){
        fprintf(stderr, "escribir_inodo()-->Error de escritura del bloque");
        return-1;
    }
     return 0;
 }

  int leer_inodo(unsigned int ninodo, struct inodo *inodo){

     struct superbloque sb;
     int bloqueAI;
     struct inodo inodos[BLOCKSIZE/INODOSIZE];

     if (bread(posSB, &sb)==-1){ //leemos el superbloque y encontramos el AI
            fprintf(stderr, "leer_inodo()-->Error de lectura del superbloque\n");
            return -1;
        }
     bloqueAI=((ninodo*INODOSIZE)/BLOCKSIZE) + sb.posPrimerBloqueAI;

     if (bread(bloqueAI,inodos)==-1) {

         fprintf(stderr, "leer_inodo()-->Error de lectura del inodo deseado\n");
         return -1;
         
     }
     *inodo = inodos[ninodo%(BLOCKSIZE/INODOSIZE)];
     return 0;
     
     
 }

 int reservar_inodo(unsigned char tipo, unsigned char permisos){
     struct superbloque sb;
     int posInodoReservado;
     struct inodo inodoReserva;
     
    if (bread(posSB, &sb)<0){
        fprintf(stderr, "reservar_inodo()-->ERROR DE LECTURA DEL SUPERBLOQUE");
        return -1;
    }
    if(sb.cantInodosLibres==0){
        fprintf(stderr, "reservar_inodo()-->NO HAY INODOS LIBRES");
        return -1;
    }
    posInodoReservado=sb.posPrimerInodoLibre;
    sb.posPrimerInodoLibre = sb.posPrimerInodoLibre+1;
     
    inodoReserva.tipo=tipo;
    inodoReserva.permisos=permisos;
    inodoReserva.nlinks = 1;
    inodoReserva.tamEnBytesLog=0;
    inodoReserva.atime=time(NULL);
    inodoReserva.ctime=time(NULL);
    inodoReserva.mtime=time(NULL);
    inodoReserva.numBloquesOcupados=0;
     
    for(int i=0;i<INDIRECTOS;i++){
        inodoReserva.punterosIndirectos[i]=0;}
    for(int i=0;i<DIRECTOS;i++){
        inodoReserva.punterosDirectos[i]=0;}
     
    if(escribir_inodo(posInodoReservado, inodoReserva)<0){
        fprintf(stderr, "reservar_inodo()-->Error de escritura en el inodo");
        return -1;
    }
    
    sb.cantInodosLibres--;
    if (bwrite(posSB, &sb)==-1){
        fprintf(stderr, "reservar_inodo()-->Error de escritura en el superbloque");
        return -1;
    }
     
    return posInodoReservado;
    

 }

int obtener_nRangoBL (struct inodo inode, unsigned int nblogico, unsigned int *ptr) {
    if (nblogico<DIRECTOS) {
        *ptr=inode.punterosDirectos[nblogico];
        return 0;
    }
    else if (nblogico<INDIRECTOS0) {
        *ptr=inode.punterosIndirectos[0];
        return 1;
    }else if  (nblogico<INDIRECTOS1)    {
        *ptr=inode.punterosIndirectos[1];
        return 2;
    }else if(nblogico<INDIRECTOS2){
        *ptr=inode.punterosIndirectos[2];
        return 3;
    }else{
        *ptr=0;
        fprintf(stderr, "obtener_nRangoBL()-->Bloque lógico fuera de rango");
        return -1;
    }
            
        
}

int obtener_indice (int nblogico,int nivel_punteros) { 
    if (nblogico<DIRECTOS) { 
        return nblogico;
    }
    else if (nblogico<INDIRECTOS0) {
        return nblogico-DIRECTOS;
    }
         
    
    else if (nblogico<INDIRECTOS1) {
        if (nivel_punteros==2) {
            return (nblogico-INDIRECTOS0)/ NPUNTEROS;
        }
        else if (nivel_punteros==1){
            return (nblogico-INDIRECTOS0)% NPUNTEROS;          
        }
    
    }
    else if (nblogico<INDIRECTOS2){
        if (nivel_punteros==3) {
            return(nblogico-INDIRECTOS1)/(NPUNTEROS*NPUNTEROS);
        }
        else if (nivel_punteros==2) {
            return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))/NPUNTEROS;
        }
        else if (nivel_punteros==1) {
            return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))%NPUNTEROS;
        }
    }     
                         
    return -1;     
            
                     
}             
              
int traducir_bloque_inodo(unsigned int ninodo,unsigned int nblogico,char reservar) {
   // Declaraciones
    struct inodo inode;
    unsigned int ptr, ptr_ant, salvar_inodo, nRangoBL, nivel_punteros, indice;
    int buffer[NPUNTEROS];  
   // fin declaraciones
   if (leer_inodo(ninodo, &inode)<0){
	fprintf(stderr, "traducir_bloque_inodo()-->Error al leer el inodo\n");
	return -1;
	}
   ptr = 0; ptr_ant = 0; salvar_inodo = 0;
   nRangoBL = obtener_nRangoBL(inode, nblogico, &ptr); //0:D, 1:I0, 2:I1, 3:I2
   nivel_punteros = nRangoBL; //el nivel_punteros +alto es el que cuelga del inodo
   while (nivel_punteros>0) { //iterar para cada nivel de indirectos
        if (ptr==0) { //no cuelgan bloques de punteros
            if (reservar==0) {
              
                return -1;
            }
            else { //reservar bloques punteros y crear enlaces desde inodo hasta datos
                salvar_inodo = 1;
                ptr = reservar_bloque(); //de punteros          
                inode.numBloquesOcupados++;
                inode.ctime = time(NULL); //fecha actual
                if (nivel_punteros == nRangoBL) { 
                    //el bloque cuelga directamente del inodo
                    inode.punterosIndirectos[nRangoBL-1] = ptr; // (imprimirlo)
                    //fprintf(stderr, "traducir_bloque_inodo()-->inodo.punterosIndirectos[%d]=%d (reservado BF %d para punteros_nivel%d)\n",nRangoBL-1,inode.punterosIndirectos[nRangoBL-1],inode.punterosIndirectos[nRangoBL-1],nivel_punteros);
                }
                else {   //el bloque cuelga de otro bloque de punteros
                    buffer[indice] = ptr; // (imprimirlo)
                    //fprintf(stderr, "traducir_bloque_inodo()-->punteros_nivel%d [%d]=%d (reservado BF %d para punteros_nivel%d)\n",nivel_punteros+1,indice,buffer[indice],buffer[indice],nivel_punteros);
                    bwrite(ptr_ant, buffer);
                }
            }
        }                
    bread(ptr, buffer);
    indice = obtener_indice(nblogico, nivel_punteros);
    ptr_ant = ptr; //guardamos el puntero
    ptr = buffer[indice] ;// y lo desplazamos al siguiente nivel
    nivel_punteros--;
   }   //al salir de este bucle ya estamos al nivel de datos
   
   if (ptr==0 ){//no existe bloque de datos 
      if (reservar==0) {
      return -1 ; //error lectura ∄ bloque  
      }  
      else{
         salvar_inodo = 1;
         ptr = reservar_bloque(); //de datos
         inode.numBloquesOcupados++;
         inode.ctime = time(NULL);
         if (nRangoBL==0 ){
            inode.punterosDirectos[nblogico] = ptr ;
            //fprintf(stderr, "traducir_bloque_inodo()-->inodo.punterosDirectos[%d]=%d (reservado BF %d para BL=%d)\n",nblogico,inode.punterosDirectos[nblogico],inode.punterosDirectos[nblogico], nblogico);
            }// (imprimirlo)
         else{
            buffer[indice] = ptr; // (imprimirlo)
            //fprintf(stderr, "traducir_bloque_inodo()-->punteros_nivel1 [%d]=%d (reservado BF %d para BL=%d)\n",indice,buffer[indice],buffer[indice],nblogico);
            bwrite(ptr_ant, buffer);
         }
      }
   }
   if (salvar_inodo==1){
      escribir_inodo(ninodo, inode);  //sólo si lo hemos actualizado
   }
   return ptr;
}

int liberar_inodo(unsigned int ninodo)
{
    struct superbloque sb;
    struct inodo inodo;
    int bloques_liberados_inodo;

    if(leer_inodo(ninodo, &inodo)<0){
	fprintf(stderr, "liberar_inodo()-->Error al leer el inodo\n");
	return -1;
	}
	
    bloques_liberados_inodo = liberar_bloques_inodo(0, inodo);
    inodo.numBloquesOcupados -= bloques_liberados_inodo;
	printf("liberar_inodo()-->liberados: %d\n", bloques_liberados_inodo);
    inodo.tipo = 'l';
    inodo.tamEnBytesLog = 0;

    if (bread(posSB, &sb) < 0)
    {
        fprintf(stderr, "liberar_inodo()-->Error de lectura del superbloque\n");
        return -1;
    }
    sb.posPrimerInodoLibre = ninodo;
    sb.cantInodosLibres++;
    escribir_inodo(ninodo, inodo);

    if (bwrite(posSB, &sb) < 0)
    {
        fprintf(stderr, "liberar_inodo()-->Error de escritura en el superbloque\n");
        return -1;
    }

    return ninodo;
}

//Se pot afegir millora de no recorrer tots es BL Peu de página(4)
int liberar_bloques_inodo(unsigned int primerBL, struct inodo inodo)
{
    // Declaración de variables
    unsigned char bufAux_punteros[BLOCKSIZE]; //1024 bytes
    unsigned int nRangoBL, nivel_punteros, indice, ptr, nblog, ultimoBL;
    int bloques_punteros[3][NPUNTEROS]; //array de bloques de punteros
    int ptr_nivel[3];                   //punteros a bloques de punteros de cada nivel
    int indices[3];                     //indices de cada nivel
    int liberados;                      // nº de bloques liberados

    
    liberados = 0;
    memset(bufAux_punteros, 0, BLOCKSIZE);
	
    if (inodo.tamEnBytesLog == 0) {
        return 0;  // el fichero vacío
    }
    //obtenemos el último bloque lógico del inodo
    if (inodo.tamEnBytesLog % BLOCKSIZE == 0) {
        ultimoBL = inodo.tamEnBytesLog / BLOCKSIZE - 1;
    }
    else {
        ultimoBL = inodo.tamEnBytesLog / BLOCKSIZE;
    }
    
    printf("liberar_bloques_inodo()-->primer BL: %d, último BL: %d\n", primerBL, ultimoBL);
    ptr = 0;

    for (nblog = primerBL; nblog <= ultimoBL; nblog++)
    { //recorrido BLs

        nRangoBL = obtener_nRangoBL(inodo, nblog, &ptr);

        if (nRangoBL < 0)
        {
            fprintf(stderr, "liberar_bloques_inodo()-->ERROR EN RECORRIDO BL");
            return -1;
        }
        nivel_punteros = nRangoBL; //el nivel_punteros +alto cuelga del inodo
	
        while (ptr > 0 && nivel_punteros > 0)
        { //cuelgan bloques de punteros
            indice = obtener_indice(nblog, nivel_punteros);
            
            if (indice == 0 || nblog == primerBL){
                bread (ptr, bloques_punteros[nivel_punteros-1]);
            }
            
            ptr_nivel[nivel_punteros - 1] = ptr;
            indices[nivel_punteros - 1] = indice;
            ptr = bloques_punteros[nivel_punteros - 1][indice];
            nivel_punteros--;
        }
        
        if (ptr > 0)
        { //si existe bloque de datos
            liberar_bloque(ptr);
            liberados++;
            fprintf(stderr, "liberar_bloques_inodo()-->liberado BF  %i de datos para BL %d\n", ptr, nblog);
            if (nRangoBL == 0)
            { //es un puntero Directo
                inodo.punterosDirectos[nblog] = 0;
            }
            else
            {
                while (nivel_punteros < nRangoBL)
                {
                    indice = indices[nivel_punteros];
                    bloques_punteros[nivel_punteros][indice] = 0;
                    ptr = ptr_nivel[nivel_punteros];

                    if (memcmp(bloques_punteros[nivel_punteros], bufAux_punteros, BLOCKSIZE) == 0)
                    {
                        //No cuelgan bloques ocupados, hay que liberar el bloque de punteros

                        liberar_bloque(ptr);
                        liberados++;
                        nivel_punteros++;
                        fprintf(stderr, "liberar_bloques_inodo()-->liberado BF %i de punteros de nivel %i correspondiente al BL %d\n", ptr, nivel_punteros, nblog);
                        if (nivel_punteros == nRangoBL)
                        {
                            inodo.punterosIndirectos[nRangoBL - 1] = 0;
                        }
                    }

                    else
                    { //escribimos en el dispositivo el bloque de punteros modificado
                        bwrite(ptr, bloques_punteros[nivel_punteros]);
                        nivel_punteros = nRangoBL; // para salir del bucle
                    }
                }
            }
        } 
    }
    printf("liberar_bloques_inodo()-->total bloques liberados: %d\n", liberados);
    return liberados;
}
 
