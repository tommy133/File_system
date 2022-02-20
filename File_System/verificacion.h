/*********************************************************/
/*Authors: Tomeu Estrany, Antonio Gaitán, Javier Santiago*/
/*********************************************************/


#include "simulacion.h"

/*
* Este script sólo existe con la finalidad de testear que todo funciona como debe.
*/
struct INFORMACION {
    int pid; 
    unsigned int nEscrituras;//validadas 
    struct REGISTRO PrimeraEscritura; 
    struct REGISTRO UltimaEscritura;
    struct REGISTRO MenorPosicion;
    struct REGISTRO MayorPosicion;
};