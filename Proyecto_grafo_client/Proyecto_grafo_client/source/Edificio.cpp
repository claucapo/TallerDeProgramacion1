#include "Edificio.h"
#include "FactoryEntidades.h"

#include <iostream>

Edificio::Edificio(unsigned int id, string name, tipoEntidad_t pType) : Entidad(id, name, pType) {
	this->tipo = ENT_T_BUILDING;
	this->cant_entrenables = pType.cantidad_entrenables;
	for (int i = 0; i < MAX_ENTRENABLES; i++) {
		this->entrenables[i] = pType.entrenables[i];
	}
	this->cola_produccion = queue<Entidad*>();
	this->ticks_restantes = 0;
	this->cant_en_produccion = 0;
	this->ticks_totales = 0;
}


// Destructor
Edificio::~Edificio() {
	while (!this->cola_produccion.empty()) {
		delete this->cola_produccion.front();
		this->cola_produccion.pop();
	}
}


bool Edificio::puedeEntrenar(string name) {
	for (int i = 0; i < this->cant_entrenables; i++) {
		if (this->entrenables[i] == name) {
			return true;
		}
	}	
	return false;
}


// Aca iria la funcion que calcula el tiempo que tarda una unidad en entrenarse
// dependiendo de las propiedades del edificio y de la entidad
void Edificio::recalcularTicks(void) {
	this->ticks_totales = 50;
	this->ticks_restantes = 50;
}


af_result_t Edificio::avanzarFrame(Escenario* scene) {
	if (this->cant_en_produccion > 0) {
		this->ticks_restantes--;
		cout << this->ticks_restantes << " ticks para crear [" << this->cola_produccion.front()->name << "]" << endl;

		// Si ya termine la unidad, devuelvo un mensaje especial
		if (this->ticks_restantes <= 0) {
			return AF_SPAWN;
		}
	}
	return AF_NONE;
}


Entidad* Edificio::obtenerUnidadEntrenada(void) {
	// Si no hay nada en produccion, devuelvo null
	if (this->cant_en_produccion <= 0)
		return nullptr;

	// Chequeo que la entidad est� terminada
	if (this->ticks_restantes <= 0) {
		Entidad* ent = this->cola_produccion.front();
		this->cola_produccion.pop();

		// Como cambio la unidad en produccion, llamo a recalcular ticks
		this->recalcularTicks();
		this->cant_en_produccion--;
		return ent;
	}
	return nullptr;
}


bool Edificio::entrenarUnidad(string name) {
	return false;
}
