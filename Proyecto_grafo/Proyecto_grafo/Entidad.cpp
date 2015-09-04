#include "Entidad.h"
#include "Posicion.h"
#include "EstadoEntidad.h"


Entidad::Entidad(Posicion p)
{

}

Entidad::Entidad(void)
{

}

Entidad::~Entidad(void)
{
}

EstadoEntidad Entidad::verEstado(void)
{
	return EstadoEntidad();
}

Posicion Entidad::verPosicion(void){
	return Posicion();
}