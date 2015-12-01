#include "Unidad.h"
#include "Posicion.h"
#include "Enumerados.h"
#include "Escenario.h"
#include "ErrorLog.h"
#include "Jugador.h"
#include "ConversorUnidades.h"
#include <cmath>
#include <iostream>

using namespace std;

Unidad::Unidad(unsigned int id, string name, tipoEntidad_t pType) : Entidad(id, name, pType) {
	this->rapidez = pType.velocidad;
	this->tipo = ENT_T_UNIT;
	this->direccion = DIR_RIGHT;
	this->destino = nullptr;

	this->collectRate = pType.collectRate;
	this->buildRate = pType.buildRate;
	this->proyectil = nullptr;
}


Unidad::~Unidad() {
	if(this->destino != nullptr)
		delete this->destino;

	if(	this->proyectil != nullptr)
		delete this->proyectil;
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
	
	// Calcular la dirección de movimiento
	float deltaX = this->destino->getX() - this->verPosicion()->getX();
	float deltaY = this->destino->getY() - this->verPosicion()->getY();
	
	this->direccion = calcularDirecion(deltaX, deltaY);
	// Asigno la direccion al sprite
	if (this->state != EST_CAMINANDO) {
		this->state = EST_CAMINANDO;
	}
}

void Unidad::setEstado(Estados_t state) {
	if((this->state == EST_CAMINANDO)&&(state == EST_QUIETO)){
		// cout << "TENGO QUE FRENAR!!!!!!!!!!!!!!!!!!!!" << endl;
	}
	this->state = state;
}

af_result_t Unidad::avanzarFrame(Escenario* scene) {
	this->sprites->siguienteFrame();
	
	return AF_NONE;
}


// Calcula la dirección del movimiento de acuerdo a las velocidades en X e Y
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

void Unidad::asignarPos(Posicion* pos){
	if(pos) {
		float xViejo = this->pos->getX();
		float yViejo = this->pos->getY();
		delete this->pos;
		this->pos = new Posicion(*pos);
		Direcciones_t dirAnt = this->direccion;
		this->direccion = this->calcularDirecion(pos->getX() - xViejo,pos->getY() - yViejo); 
		// cout << "Direc: " << this->direccion << endl;
		if(this->direccion != dirAnt)
			if(this->sprites)
				sprites->cambiarSubImagen(0, this->direccion);
		//cout<<"Dir:" << pos->getX() - xViejo <<"-"<<pos->getY() - yViejo<<endl;
	}
	else
		ErrorLog::getInstance()->escribirLog("Error al querer asignar Posicion a " + this->name + ": Posicion inexistente.", LOG_ERROR);
}