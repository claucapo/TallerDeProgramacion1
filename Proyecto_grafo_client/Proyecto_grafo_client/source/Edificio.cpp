#include "Edificio.h"
#include "FactoryEntidades.h"

#include <iostream>

#include "ErrorLog.h"
#include <sstream>

Edificio::Edificio(unsigned int id, string name, tipoEntidad_t pType) : Entidad(id, name, pType) {
	this->tipo = ENT_T_BUILDING;
	this->cant_entrenables = pType.cantidad_entrenables;
	for (int i = 0; i < MAX_ENTRENABLES; i++) {
		this->entrenables[i] = pType.entrenables[i];
	}

	this->cola_produccion = queue<training_dummy>();
	this->ticks_restantes = 0;
	this->cant_en_produccion = 0;
	this->ticks_totales = 0;
}


// Destructor
Edificio::~Edificio() {
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
	this->porcentajeEntrenado();
	return AF_NONE;
}


bool Edificio::entrenarUnidad(unsigned int typeID, int totalTicks) {
	training_dummy td;
	td.name = FactoryEntidades::obtenerInstancia()->obtenerName(typeID);
	td.typeID = typeID;
	td.totalTicks = totalTicks;
	if (this->cant_en_produccion < MAX_PRODUCCION) {
		this->cola_produccion.push(td);
		this->cant_en_produccion++;

		if (this->cant_en_produccion == 1)
			this->resetearTicks();
		return true;
	}
	return false;
}

void Edificio::finalizarEntrenamiento(void) {
	stringstream s;
	s << "Ending production of [" << this->cola_produccion.front().name << "].";
	ErrorLog::getInstance()->escribirLog(s.str());

	if (this->cant_en_produccion > 0) {
		this->cola_produccion.pop();
		this->cant_en_produccion--;
		this->resetearTicks();
	
	}
}

void Edificio::resetearTicks(void) {
	if (this->cant_en_produccion > 0) {
		this->ticks_restantes = this->cola_produccion.front().totalTicks;
		this->ticks_totales = this->ticks_restantes;
	}
}

float Edificio::porcentajeEntrenado(void) {
	if (this->cant_en_produccion <= 0)
		return -1;

	
	stringstream s;
	s << "Producing [" << this->cola_produccion.front().name << "]. Done " << (float)ticks_restantes/ticks_totales;
	ErrorLog::getInstance()->escribirLog(s.str());

	return 1 - ticks_restantes/ticks_totales;
}


void Edificio::setEnConstruccion(bool construyendo) {
	if (construyendo) {
		this->tipo = ENT_T_CONSTRUCTION;
		this->vidaAct = 1;
	} else {
		this->tipo = ENT_T_BUILDING;
	}
}