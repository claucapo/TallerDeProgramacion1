#include "Recurso.h"
#include "Escenario.h"
#include "Jugador.h"
#include <iostream>

using namespace std;

Recurso::Recurso(void) : Entidad() {
	this->amount = 1;
	this->tipo = ENT_T_RESOURCE;
}

Recurso::Recurso(int amount) : Entidad() {
	if (amount >= 0)
		this->amount = amount;
	else
		this->amount = 1;
	this->tipo = ENT_T_RESOURCE;
}

Recurso::Recurso(unsigned int id, string name, int tamX, int tamY, int vision, int amount) : Entidad(id, name, tamX, tamY, vision) {
	if (amount >= 0)
		this->amount = amount;
	else
		this->amount = 1;
	this->tipo = ENT_T_RESOURCE;
}

af_result_t Recurso::avanzarFrame(Escenario* scene) {
	this->verSpritesheet()->siguienteFrame();
	return AF_NONE;
}
