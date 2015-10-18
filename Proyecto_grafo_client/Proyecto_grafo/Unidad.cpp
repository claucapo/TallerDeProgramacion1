#include "Unidad.h"
#include "Posicion.h"
#include "Enumerados.h"
#include "ErrorLog.h"
#include "ConversorUnidades.h"
#include <cmath>
#include <iostream>

Unidad::Unidad() : Entidad() {
	this->rapidez = 0;
	this->direccion = DIR_DOWN;
	this->destino = nullptr;
}

Unidad::Unidad(unsigned int id, string name, int tamX, int tamY, int vision, int velocidad) : Entidad(id, name, tamX, tamY, vision) {
	this->rapidez = velocidad;
	this->direccion = DIR_DOWN;
	this->destino = nullptr;
}

Unidad::Unidad(Posicion* p) : Entidad() {
	if (p) {
		this->pos = p;
		this->destino = new Posicion(this->pos->getX(), this->pos->getY());
	} else {
		ErrorLog::getInstance()->escribirLog("Error al querer asignar Posicion a " + this->name + ": Posicion inexistente.", LOG_ERROR);
		this->destino = nullptr;
	}
	this->rapidez = 0;
	this->direccion = DIR_DOWN;
	}

Unidad::~Unidad() {
	delete this->destino;
}

// Se espera la velocidad en UL/frames!
void Unidad::setVelocidad(float nuevaVelocidad) {
	this->rapidez = nuevaVelocidad;
}

// Designa un nuevo destino, cambiando la direccion del movimiento
void Unidad::nuevoDestino(Posicion* pos){
	// Asigno el nuevo destino
	if(this->destino)
		delete this->destino;
	this->destino = new Posicion(*pos);
	
	// Calcular la direcci�n de movimiento
	float deltaX = this->destino->getX() - this->verPosicion()->getX();
	float deltaY = this->destino->getY() - this->verPosicion()->getY();
	
	this->direccion = calcularDirecion(deltaX, deltaY);
	// Asigno la direccion al sprite
	if (this->state != EST_CAMINANDO) {
		this->state = EST_CAMINANDO;
	}
}

void Unidad::setEstado(Estados_t state) {
	this->state = state;
}

af_result_t Unidad::avanzarFrame(Escenario* scene) {
	// Aca habr�a que chequear si la entidad cambi� de posici�n

	Estados_t state = this->state;

	// Si se esta moviendo
	if (state == EST_CAMINANDO) {
		Posicion* act = this->pos;
		Posicion* dest = this->destino;

		// Distantcias
		float distX = dest->getX() - act->getX();
		float distY = dest->getY() - act->getY();
		float totalDist = sqrt((distX*distX) + (distY*distY));	

		this->setDireccion(this->calcularDirecion(distX, distY));
		
		if (totalDist > this->rapidez) {
			float nuevoX = act->getX() + (distX*rapidez)/totalDist;
			float nuevoY = act->getY() + (distY*rapidez)/totalDist;
			Posicion nuevaPos(nuevoX, nuevoY);
			this->asignarPos(&nuevaPos);
		} else {
			this->setEstado(EST_QUIETO);
		}
		return AF_MOVE;
	}

	return AF_NONE;
}


// Calcula la direcci�n del movimiento de acuerdo a las velocidades en X e Y
Direcciones_t Unidad::calcularDirecion(float velocidadX, float velocidadY) {
	
	// Transformo a cartesianas comunes
	float vX = velocidadY * 0.866 - velocidadX * 0.866;
	float vY = velocidadY * (-0.5) - velocidadX * 0.5;
	
	if(vX > 0){
		float tang = vY/vX;
		if(tang > 2.414)
			return DIR_TOP;
		else if(tang > 0.414)
			return DIR_TOP_RIGHT;
		else if(tang < -2.414)
			return DIR_DOWN;
		else if(tang < -0.414)
			return DIR_DOWN_RIGHT;
		else
			return DIR_RIGHT;
		}
	if(vX < 0){
		float tang = - vY/vX;
		if(tang > 2.414)
			return DIR_TOP;
		else if(tang > 0.414)
			return DIR_TOP_LEFT;
		else if(tang < -2.414)
			return DIR_DOWN;
		else if(tang < -0.414)
			return DIR_DOWN_LEFT;
		else
			return DIR_LEFT;
		}
	if(vY > 0)
		return DIR_TOP;
	return DIR_DOWN;
	}


void Unidad::setDireccion(Direcciones_t dir) {
	direccion = dir;
}
