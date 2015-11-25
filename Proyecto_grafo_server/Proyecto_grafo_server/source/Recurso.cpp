#include "Recurso.h"
#include "Escenario.h"
#include "Jugador.h"
#include <iostream>

using namespace std;

Recurso::Recurso(unsigned int id, string name, tipoEntidad_t pType) : Entidad(id, name, pType) {
	this->recursoMax = pType.recursoMax;
	this->recursoAct = pType.recursoMax;

	this->tipoR = pType.tipoR;
}

af_result_t Recurso::avanzarFrame(Escenario* scene) {
	if (this->state == EST_MUERTO || this->recursoAct == 0) {
		return AF_KILL;
	}	
	return AF_NONE;
}
