#include "Edificio.h"
#include "FactoryEntidades.h"
#include "Escenario.h"

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
}

bool Edificio::puedeEntrenar(unsigned int id) {
	for (int i = 0; i < this->cant_entrenables; i++) {
		if (this->entrenables[i] == FactoryEntidades::obtenerInstancia()->obtenerName(id)) {
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
		
		msg_update* upd = scene->generarUpdate(MSJ_AVANZAR_PRODUCCION, this->verID(), -1, 0);
		scene->updatesAux.push_back(upd);

		// Si ya termine la unidad, devuelvo un mensaje especial
		if (this->ticks_restantes <= 0) {
			msg_update* upd = scene->generarUpdate(MSJ_FINALIZAR_PRODUCCION, this->verID(), 0, 0);
			scene->updatesAux.push_back(upd);
			return AF_SPAWN;
		}
	}
	if (this->vidaAct <= 0 || this->state == EST_MUERTO)
		return AF_KILL;

	return AF_NONE;
}


Entidad* Edificio::obtenerUnidadEntrenada(void) {
	if (this->tipo == ENT_T_CONSTRUCTION)
		return nullptr;

	// Si no hay nada en produccion, devuelvo null
	if (this->cant_en_produccion <= 0)
		return nullptr;

	// Chequeo que la entidad esté terminada
	if (this->ticks_restantes <= 0) {
		Entidad* ent = this->cola_produccion.front();
		this->cola_produccion.pop();

		ent->asignarPos(new Posicion(this->verPosicion()->getX(), this->verPosicion()->getY()));
		ent->asignarJugador(this->verJugador());

		// Como cambio la unidad en produccion, llamo a recalcular ticks
		this->recalcularTicks();
		this->cant_en_produccion--;
		return ent;
	}
	return nullptr;
}


bool Edificio::entrenarUnidad(string name) {
	return this->entrenarUnidad(FactoryEntidades::obtenerInstancia()->obtenerTypeID(name));
}



bool Edificio::entrenarUnidad(unsigned int id) {
	if (this->tipo == ENT_T_CONSTRUCTION)
		return false;

	// No puedo entrenar a la unidad si no está en mi lista de entrenables o si el edificio está saturado
	if (!this->puedeEntrenar(id) || (this->cant_en_produccion == MAX_PRODUCCION)) {
		cout << "No se puede entrenar [" << id << "]" << endl;
		return false;
	}

	Entidad* ent = FactoryEntidades::obtenerInstancia()->obtenerEntidad(id);
	if (!ent)
		return false;

	this->cola_produccion.push(ent);
	this->cant_en_produccion++;

	// Si el edificio estaba vacío, recalculo los ticks
	if (this->cant_en_produccion == 1)
		this->recalcularTicks();
	return true;
}


void Edificio::setEnConstruccion(bool construyendo) {
	if (construyendo) {
		this->tipo = ENT_T_CONSTRUCTION;
		this->vidaAct = 1;
	} else {
		this->tipo = ENT_T_BUILDING;
	}
}
