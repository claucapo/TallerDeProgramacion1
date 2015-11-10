#include "Entidad.h"
#include "Posicion.h"
#include "Enumerados.h"
#include "ErrorLog.h"
#include <iostream>

Entidad::Entidad(unsigned int id, string name, tipoEntidad_t pType) {
	this->id = id;
	this->name = name;
	this->pos = nullptr;
	this->owner = nullptr;
	this->state = EST_QUIETO;

	this->accion = ACT_NONE;
	this->targetID = this->id;

	this->tamX = (pType.tamX > 0) ? pType.tamX : 1;
	this->tamY = (pType.tamY > 0) ? pType.tamY : 1;
	this->rangoVision = pType.rangoV;

	this->vidaAct = pType.vidaMax;
	this->vidaMax = pType.vidaMax;
	this->ataque = pType.ataque;
	this->defensa = pType.defensa;

	this->cooldownMax = pType.cooldown;
	this->cooldownAct = 0;
	for (int i = 0; i < CANT_ACCIONES; i++) {
		this->habilidades[i] = pType.habilidades[i];		
	}
}

// Llamo al destructor de todos los miembros de la clase (en caso de que alguno
// necesitara librar memoria)
Entidad::~Entidad(void) {
	if (this->pos)
		delete this->pos;
}

// Método para avanzar un frame
// Devuelve true si hay que quitar la entidad del escenario
af_result_t Entidad::avanzarFrame(Escenario* scene) {
	if (this->vidaAct <= 0 || this->state == EST_MUERTO)
		return AF_KILL;
	return AF_NONE;
}

void Entidad::asignarAccion(Accion_t acc, unsigned int targetID) {
	// La entidad genérica no puede poseer acciones (por ahora?)
	this->accion = ACT_NONE;
	targetID = 0;
}


void Entidad::asignarPos(Posicion* pos) {
	if(pos) {
		delete this->pos;
		this->pos = new Posicion(*pos);		
	}
	else
		ErrorLog::getInstance()->escribirLog("Error al querer asignar Posicion a " + this->name + ": Posicion inexistente.", LOG_ERROR);
}

void Entidad::asignarJugador(Jugador* player) {
	if (!player)
		ErrorLog::getInstance()->escribirLog("Error al querer asignar un jugador a " + this->name + ": Jugador inexistente.", LOG_ERROR);
	else
		this->owner = player;
}

void Entidad::asignarEstado(Estados_t state) {
	this->state = state;
}

bool Entidad::operator==(Entidad other){
	return ((this->pos == other.verPosicion()) && (this->state == other.verEstado()) );
}

// Magic!!!
bool Entidad::operator < (const Entidad& other) const {
	int max_x_A = this->pos->getRoundX() + this->tamX;
	int max_y_A = this->pos->getRoundY() + this->tamY;
	int min_x_A = this->pos->getRoundX();
	int min_y_A = this->pos->getRoundY();
	
	int max_x_B = other.pos->getRoundX() + other.tamX;
	int max_y_B = other.pos->getRoundY() + other.tamY;
	int min_x_B = other.pos->getRoundX();
	int min_y_B = other.pos->getRoundY();

	if (min_x_B < max_x_A && min_y_B < max_y_A)
		return false;

	if (min_x_B > min_x_A && min_y_B > max_y_B)
		return true;

	if (min_x_B >= max_x_A)
		if (max_y_B > min_y_A)
			return true;

	if (min_y_B >= max_y_A)
		if (max_x_B > min_x_A)
			return true;

	if (min_x_B + min_y_B > min_x_A + min_y_A)
		return true;
	else if (min_x_B + min_y_B == min_x_A + min_y_A)
		return min_x_B > min_x_A;
	else
		return false;

}