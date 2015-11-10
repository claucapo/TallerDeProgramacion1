#include "Recurso.h"
#include "Escenario.h"
#include "Jugador.h"
#include <iostream>

using namespace std;

Recurso::Recurso(unsigned int id, string name, tipoEntidad_t pType) : Entidad(id, name, pType) {
	this->tipo = ENT_T_RESOURCE;
	this->tipoR = tipoR;
	this->recursoMax = pType.recursoMax;
	this->recursoAct = this->recursoMax;
}


af_result_t Recurso::avanzarFrame(Escenario* scene) {
	this->verSpritesheet()->siguienteFrame();
	return AF_NONE;
}
