#include "Factory.h"


Entidad* Factory::obtenerEntidad(instanciaInfo_t* instanciaInfo){	
	if(instanciaInfo->tipo == "house")
		return new Casa();
	else if(instanciaInfo->tipo == "town_center")
		return new CentroUrbano();
	else if(instanciaInfo->tipo == "castle")
		return new Castillo();
	else if(instanciaInfo->tipo == "windmill")
		return new Molino();
	else if(instanciaInfo->tipo == "stable")
		return new Establo();
	else if(instanciaInfo->tipo == "barracks")
		return new Cuarteles();
	else if(instanciaInfo->tipo == "archery")
		return new Arqueria();
}

Unidad* Factory::obtenerUnidad(instanciaInfo_t* instanciaInfo){	
	if(instanciaInfo->tipo == "settler")
		return new Aldeano();
	if(instanciaInfo->tipo == "champion")
		return new Campeon();
	if (instanciaInfo->tipo == "knight")
		return new Jinete();
};