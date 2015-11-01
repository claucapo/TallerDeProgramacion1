#include "Unidad.h"
#include "Posicion.h"
#include "Enumerados.h"
#include "Escenario.h"
#include "ErrorLog.h"
#include "ConversorUnidades.h"
#include <cmath>
#include <iostream>
#include <list>

Unidad::Unidad() : Entidad() {
	this->rapidez = 0;
	this->direccion = DIR_DOWN;
	this->destino = nullptr;
}

Unidad::Unidad(unsigned int id, string name, int tamX, int tamY, int vision, int velocidad) {
	this->pos = nullptr;
	this->id = id;
	this->name = name;
	this->state = EST_QUIETO;
	this->tamX = (tamX > 0) ? tamX : 1;
	this->tamY = (tamY > 0) ? tamY : 1;
	this->rangoVision = vision;

	this->owner = nullptr;

	this->rapidez = (float)velocidad/100;
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

void Unidad::marcarCamino(list<Posicion*> camino) {
	this->camino = camino;
	this->camino.push_back(this->destino);
}

bool estaEnCasilla(Posicion* punto, Posicion* contenedor){
	int cX = contenedor->getRoundX();
	int cY = contenedor->getRoundY();
	if((punto->getX() >= cX) && (punto->getX() <= (cX+1)))
		if((punto->getY() >= cY) && (punto->getY() <= (cY+1)))
			return true;
	return false;
}


af_result_t Unidad::avanzarFrame(Escenario* scene) {
	// Aca habr�a que chequear si la entidad cambi� de posici�n

	Estados_t state = this->state;

	//el ultimo elemento es el destino original
	if (camino.size() == 0){
		this->setEstado(EST_QUIETO);
	}

	if ((!camino.empty()) && (state == EST_CAMINANDO)) {
		Posicion* act = this->pos;
		Posicion* dest = this->camino.front();


		// Distantcias
		float distX = dest->getX() - act->getX() ;
		float distY = dest->getY() - act->getY() ;
		float totalDist = sqrt((distX*distX) + (distY*distY));	

		this->setDireccion(this->calcularDirecion(distX, distY));
		
		if ((totalDist > this->rapidez) || (dest->getX() > act->getX())) {
 			float nuevoX = act->getX() + (distX*rapidez)/totalDist;
 			float nuevoY = act->getY() + (distY*rapidez)/totalDist;
 			Posicion nuevaPos(nuevoX, nuevoY);
			if(estaEnCasilla(&nuevaPos, act) || estaEnCasilla(&nuevaPos, dest)){
			//	scene->verMapa()->quitarEntidad(this);
			//	scene->verMapa()->vaciarPosicionSinChequeo(act);
				
				this->asignarPos( &nuevaPos);
			//	scene->verMapa()->ubicarEntidad(this, &nuevaPos);
			//	scene->verMapa()->ocuparPosicionSinChequeo(&nuevaPos, this);
			
				}
 			else {
				if(scene->verMapa()->posicionEstaVacia(&nuevaPos)){
				//	scene->verMapa()->vaciarPosicionSinChequeo(act);
				
				//	scene->verMapa()->quitarEntidad(this);
					this->asignarPos( &nuevaPos);
				//	scene->verMapa()->ubicarEntidad(this, &nuevaPos);
				//	scene->verMapa()->ocuparPosicionSinChequeo(&nuevaPos, this);
			
					}
				else{
					// Si la posicion que atravieso no est� vac�a...
				/*	while(!this->camino.empty())
						this->camino.pop_front();
					this->asignarPos(new Posicion(act->getRoundX() + 0.5, act->getRoundY() + 0.5));
					this->setEstado(EST_QUIETO);*/
					Posicion aux(nuevaPos.getX(), nuevaPos.getY());
					if((this->direccion == DIR_RIGHT)){
						do
						aux = Posicion(aux.getX() +0.35, aux.getY() +0.45);
						while(aux == nuevaPos);
					}
					else if((this->direccion == DIR_LEFT)){
						do
						aux = Posicion(aux.getX() +0.45, aux.getY() +0.35);
						while(aux == nuevaPos);
					}
					else if((this->direccion == DIR_DOWN_LEFT)){
						do
						aux = Posicion(aux.getX() +0.1, aux.getY() +0.35);
						while(aux == nuevaPos);
					}
					else if((this->direccion == DIR_DOWN_LEFT)){
						do
						aux = Posicion(aux.getX() +0.35, aux.getY() +0.1);
						while(aux == nuevaPos);
					}
					
				/*	else{
						
				//		while(!camino.empty())
				//			this->camino.pop_front();
				//		this->setEstado(EST_QUIETO);
						
						scene->asignarDestino(this->id, *camino.back());
						aux= Posicion(act->getRoundX()+0.44,act->getRoundY()+0.44);
						}*/
					
				//	scene->verMapa()->quitarEntidad(this);
				
					nuevaPos = Posicion(aux.getX(), aux.getY());
					scene->moverEntidad(this, &nuevaPos);
				//	scene->verMapa()->ubicarEntidad(this, new Posicion(act->getRoundX()+0.44,act->getRoundY()+0.44));
			
				}
			}
 		} else {
			this->camino.pop_front();
			if (camino.size() == 0){
				this->setEstado(EST_QUIETO);
			//	scene->verMapa()->quitarEntidad(this);
			//	scene->verMapa()->vaciarPosicionSinChequeo(act);
				this->asignarPos( new Posicion(act->getRoundX()+0.44,act->getRoundY()+0.44));
			//	scene->verMapa()->ubicarEntidad(this, new Posicion(act->getRoundX()+0.44,act->getRoundY()+0.44));
			//	scene->verMapa()->ocuparPosicionSinChequeo( new Posicion(act->getRoundX()+0.44,act->getRoundY()+0.44), this);
			}

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
