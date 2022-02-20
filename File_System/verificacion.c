/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "verificacion.h"


int main(int argc, char **argv) {
	char *disco = argv[1];
	char *dir_simul = argv[2];
	if (argc != 3) {
		fprintf(stderr,"Sintaxis: ./verificacion <disco> <directorio_simulacion>\n");
		return -1;
	}

	bmount(disco);

	struct STAT stat;
 	if (mi_stat(dir_simul, &stat) == -1) { // Obtenemos la metainformacion del directorio de simulacion
		printf("Error al obtener la metainformacion del directorio de simulacion\n");
		return -1;
	}

	
	int numentradas = stat.tamEnBytesLog / sizeof(struct entrada);
	struct entrada entradas[numentradas];
	if (mi_read(dir_simul, entradas, 0, sizeof(entradas)) == -1) { // Leemos las entradas del directorio de simulación
		printf("Error al leer las entradas del directorio de simulacion\n");
		return -1;
	}

	char path_informe[128];
	sprintf(path_informe, "%s%s", dir_simul, "informe.txt");
	
	if (mi_creat(path_informe, 7) == -1) {// Creamos el fichero de informe
		printf("Error al crear el fichero informe\n");
		return -1;
	}
	
	int i, nbytes_informe = 0;
	for (i = 0; i < numentradas; i++) {
		pid_t pid = atoi(strchr(entradas[i].nombre, '_') + 1);

		int num_escrituras = 0;
		struct INFORMACION info;
		char path_fichero[128];
		sprintf(path_fichero, "%s%s/%s", dir_simul, entradas[i].nombre, "prueba.dat");
		int offset = 0;
		struct REGISTRO registros[(BLOCKSIZE/sizeof(struct REGISTRO))*200];
		while (mi_read(path_fichero, registros, offset, sizeof(registros)) > 0) {
			int j;
			for (j = 0; j < (BLOCKSIZE/sizeof(struct REGISTRO))*200; j++) {
				if (registros[j].pid == pid) {
					if (num_escrituras == 0) {
						info.PrimeraEscritura = registros[j];
						info.UltimaEscritura = registros[j];
						info.MenorPosicion = registros[j];
						info.MayorPosicion = registros[j];
					} else {
						if(difftime(info.PrimeraEscritura.fecha, registros[j].fecha) > 0){
                     info.PrimeraEscritura = registros[j];
                  }else if((difftime(info.PrimeraEscritura.fecha, registros[j].fecha)  == 0)
                           && registros[j].nEscritura < info.PrimeraEscritura.nEscritura ){
                     info.PrimeraEscritura = registros[j];
                  }else if(difftime(info.UltimaEscritura.fecha, registros[j].fecha) < 0){
                     info.UltimaEscritura = registros[j];
                  }else if ((difftime(info.UltimaEscritura.fecha, registros[j].fecha)  == 0)
                           && registros[j].nEscritura > info.UltimaEscritura.nEscritura ){
                     info.UltimaEscritura = registros[j];
                  }
					}
					num_escrituras++;
				}
			}
			memset(registros, 0, sizeof(registros));
			offset += sizeof(registros);
		}
		
		printf("%d escrituras validadas en %s\n", num_escrituras, path_fichero);

		char buffer[BLOCKSIZE];
		memset(buffer, 0, BLOCKSIZE);
		sprintf(buffer, "PID: %i\nNumero de escrituras: %i\n",  pid, num_escrituras);

		sprintf(buffer + strlen(buffer), "%s %i %i %s","Primera escritura: ",info.PrimeraEscritura.nEscritura,info.PrimeraEscritura.nRegistro, asctime(localtime(&info.PrimeraEscritura.fecha)));

		sprintf(buffer + strlen(buffer), "%s %i %i %s","Ultima escritura: ",info.UltimaEscritura.nEscritura,info.UltimaEscritura.nRegistro,asctime(localtime(&info.UltimaEscritura.fecha)));

		sprintf(buffer + strlen(buffer), "%s %i %i %s","Menor posicion: ",info.MenorPosicion.nEscritura,info.MenorPosicion.nRegistro,asctime(localtime(&info.MenorPosicion.fecha)));

		sprintf(buffer + strlen(buffer), "%s %i %i %s %s","Mayor posicion: ",info.MayorPosicion.nEscritura,info.MayorPosicion.nRegistro,asctime(localtime(&info.MayorPosicion.fecha)),"\n");

		if (mi_write(path_informe, buffer, nbytes_informe, strlen(buffer)) == -1) {
			printf("Error al escribir en el informe\n");
		}
		nbytes_informe += strlen(buffer);
	}
	bmount(disco);

	return 0;
}