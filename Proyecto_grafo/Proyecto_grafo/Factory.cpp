#include "Factory.h"


Entidad* Factory::obtenerEntidad(instanciaInfo_t* instanciaInfo){	
	if(instanciaInfo->tipo == "house")
		return new Casa();
	if(instanciaInfo->tipo == "town_center")
		return new CentroUrbano();
}

Unidad* Factory::obtenerUnidad(instanciaInfo_t* instanciaInfo){	
	if(instanciaInfo->tipo == "settler")
		return new Aldeano();
	if(instanciaInfo->tipo == "champion")
		return new Campeon();
	if (instanciaInfo->tipo == "knight")
		return new Jinete();
};