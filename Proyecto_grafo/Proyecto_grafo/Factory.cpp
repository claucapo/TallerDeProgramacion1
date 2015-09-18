#include "Factory.h"


Entidad* Factory::obtenerEntidad(instanciaInfo_t* instanciaInfo)
{
	
	if(instanciaInfo->tipo == "casa")
		return new Casa();
	if(instanciaInfo->tipo == "centro_urbano")
		return new CentroUrbano();

};