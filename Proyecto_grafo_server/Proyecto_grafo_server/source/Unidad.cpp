#include "Unidad.h"
#include "Posicion.h"
#include "Enumerados.h"
#include "Escenario.h"
#include "Jugador.h"
#include "ErrorLog.h"
#include "Recurso.h"
#include "ConversorUnidades.h"
#include <cmath>
#include <iostream>
#include <list>


Unidad::Unidad(unsigned int id, string name, tipoEntidad_t pType) : Entidad(id, name, pType) {
	this->rapidez = (float)pType.velocidad/100;
	this->direccion = DIR_DOWN;
	this->destino = nullptr;

	this->collectRate = pType.collectRate;
	this->buildRate = pType.buildRate;
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




bool Unidad::puedeRealizarAccion(Accion_t acc) {
	// Insertar la l�gica que decida si se puede o no realizar la accion
	return this->habilidades[acc];
}

void Unidad::asignarAccion(Accion_t acc, unsigned int targetID) {
	if (this->puedeRealizarAccion(acc)) {
		this->state = EST_QUIETO;
		this->accion = acc;
		this->targetID = targetID;
	} else {
		cout << this->name << " cannot perform " << acc << endl;
	}
}

bool Unidad::objetivoEnRango(Entidad* target, Escenario* scene) {
	Posicion* targetPos = target->verPosicion();
	int distX = targetPos->getRoundX() - this->pos->getRoundX();
	int distY = targetPos->getRoundY() - this->pos->getRoundY();

	if ( distX >= -1 && distX <= 1 )
		if ( distY >= -1 && distY <= 1 )
			return true;

	return false;
}




void Unidad::mover(Escenario* scene) {
	Posicion* act = this->pos;
	Posicion* dest = this->camino.front();

	if (scene->verMapa()->verContenido(dest) != this && this->camino.size() > 1) {
		if (!scene->casillaEstaVacia(dest)) {
			cout << "Hmmm... algo no me deja pasar" << endl;
			scene->asignarDestino(this->verID(), *this->verDestino());
			return;
		}
	}

	// Distantcias ---- le agruegu� el +0.5
	float distX = dest->getX() - act->getX() + 0.5;
	float distY = dest->getY() - act->getY() + 0.5;
	float totalDist = sqrt((distX*distX) + (distY*distY));	

	this->setDireccion(this->calcularDirecion(distX, distY));
		
	if ((totalDist > this->rapidez) || (dest->getX() > act->getX())) {
 		float nuevoX = act->getX() + (distX*rapidez)/totalDist;
 		float nuevoY = act->getY() + (distY*rapidez)/totalDist;
 		Posicion nuevaPos(nuevoX, nuevoY);
		if( !(estaEnCasilla(&nuevaPos, act) || estaEnCasilla(&nuevaPos, dest)) ) {
			if ( !(scene->verMapa()->posicionEstaVacia(&nuevaPos)) ) {
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
				nuevaPos = Posicion(aux.getX(), aux.getY());
			}
		}
		scene->moverEntidad(this, &nuevaPos);
 	} else {
		this->camino.pop_front();
		if (camino.size() == 0){
			// this->setEstado(EST_QUIETO);
			// this->asignarPos( new Posicion(act->getRoundX()+0.44,act->getRoundY()+0.44));
			Posicion aux(act->getRoundX()+0.44,act->getRoundY()+0.44);
			scene->moverEntidad(this, &aux);
		}
 	}
}

bool Unidad::resolverAtaque(Entidad* target, Escenario* scene) {
	target->vidaAct -= this->ataque;
	cout << this->verID() << " ataca a " << target->verID() << " y lo deja en " << target->vidaAct << " de vida." << endl;

	msg_update* upd = scene->generarUpdate(MSJ_VIDA_CHANGE, target->verID(), this->ataque, 0);
	scene->updatesAux.push_back(upd);

	if (target->vidaAct <= 0) {
		target->asignarEstado(EST_MUERTO);
		cout << target->verID() << " debe morir!!" << endl;
		return false;
	}
	return true;
}

bool Unidad::resolverRecoleccion(Entidad* target, Escenario* scene) {
	if (target->tipo != ENT_T_RESOURCE)
		return false;
	Recurso* res = (Recurso*)target;
	int recolectado = 0;
	if (res->recursoAct > this->collectRate) {
		recolectado = this->collectRate;
	} else {
		recolectado = res->recursoAct;
	}
	res->recursoAct -= recolectado;
	
	msg_update* upd = scene->generarUpdate(MSJ_RES_CHANGE, target->verID(), recolectado, 0);
	scene->updatesAux.push_back(upd);

	this->verJugador()->modificarRecurso(res->tipoR, recolectado);

	/*
	Jugador* player = this->verJugador();
	cout << player->verNombre() << " tiene " << player->verRecurso().oro << " oro, ";
	cout << player->verRecurso().comida << " comida, ";
	cout << player->verRecurso().madera << " madera y ";
	cout << player->verRecurso().piedra << " piedra." << endl;
	*/

	if (res->recursoAct == 0) {
		res->asignarEstado(EST_MUERTO);
		return false;
	} else {
		return true;
	}
	return true;
}

bool Unidad::resolverConstruccion(Entidad* target, Escenario* scene) {
	return false;
}

bool Unidad::realizarAccion(Accion_t acc, Entidad* target, Escenario* scene) {
	if (this->cooldownAct != 0) return true;
	
	bool ret_val = false;
	switch (acc) {
	case ACT_ATACK:
		ret_val = resolverAtaque(target, scene); break;
	case ACT_COLLECT:
		ret_val = resolverRecoleccion(target, scene); break;
	case ACT_BUILD:
		ret_val = resolverConstruccion(target, scene); break;
	}
	this->cooldownAct = this->cooldownMax;

//	if (ret_val)
//		cout << "I am at: " << this->pos->toStr() << endl;

	return ret_val;
}


/* Tabla de estados y acciones:
	EST_QUIETO + ACT_NONE --> nop
	EST_MOVER  + ACT_NONE --> llegu�_a_destino ? (state <- EST_QUIETO) : mover
	EST_XXXX   + ACT_NONE --> (state <- EST_QUIETO)

	EST_QUIETO + ACT_XXXX --> objetivo_en_rango ? (state <- EST_XXXX) : (state <- EST_MOVER)
	EST_MOVER  + ACT_XXXX --> objetivo_en_rango ? (state <- EST_XXXX) : mover
	EST_XXXX   + ACT_XXXX --> objetivo_en_rango ? (realizar_accion) : (state <- EST_MOVER)
	EST_YYYY   + ACT_XXXX --> objetivo_en_rango ? (state <- EST_XXXX) : (state <- EST_MOVER)

	IMPORTANTE: tener en cuenta que el estado es lo que la unidad est� haciendo en el momento,
				mientras que la acci�n es la tarea que tiene asignada la unidad.
*/
af_result_t Unidad::avanzarFrame(Escenario* scene) {
	if (this->id == 4 && this->camino.size() <= 1) {
		int i = 0;
	}
	Estados_t state = this->state;
	Accion_t accion = this->accion;
	if (this->cooldownAct > 0) this->cooldownAct--;

	if (state == EST_MUERTO)
		return AF_KILL;

	if (accion == ACT_NONE) {
		if (state == EST_CAMINANDO) {
			// Si la unidad esta caminado, primero pregunto si llegu� al destino
			if (camino.size() == 0) {
				this->asignarEstado(EST_QUIETO);
				return AF_STATE_CHANGE;
			} else {
				this->mover(scene);
 				return AF_MOVE;
			}
		} else {
			// Si la unidad estaba haciendo cualquier otra cosa, altero el estado para que se detenga
			if(this->verEstado() != EST_QUIETO) {
				this->asignarEstado(EST_QUIETO);
				return AF_STATE_CHANGE;
			} else {
				return AF_NONE;
			}
		}

	} else {
		Entidad* target = scene->obtenerEntidad(this->targetID);
		if (!target) {
			this->asignarAccion(ACT_NONE, 0);
			this->asignarEstado(EST_QUIETO);
			return AF_STATE_CHANGE;
		}
		bool enRango = this->objetivoEnRango(target, scene);
		if (!enRango) {
			if (state == EST_CAMINANDO) {
				this->mover(scene);
 				return AF_MOVE;
			} else {

				// WARNING: hardcodeo...
				// Aca se tendria que mandar a la entidad a una posici�n adyacente, y no a la hardcodeada
				int x = target->verPosicion()->getRoundX();
				int y = target->verPosicion()->getRoundY();
				for (int i = -1; i <= 1; i++) {
					for (int j = -1; j <= 1; j++) {
						Posicion pos(target->verPosicion()->getRoundX()+i, target->verPosicion()->getRoundY()+j);
						if (scene->casillaEstaVacia(&pos)) {
							this->asignarEstado(EST_CAMINANDO);
							scene->asignarDestino(this->verID(), pos);
							// cout << "Asigne un path a: " << pos.toStrRound() << endl;
							return AF_STATE_CHANGE;
						}
					}
				}
				
				this->state = EST_QUIETO;
				this->accion = ACT_NONE;
				return AF_NONE;

			}
		} else {
			if (state == accionAEstado[accion]) {
				bool continuar = this->realizarAccion(accion, target, scene);
				if (!continuar) {
					this->asignarAccion(ACT_NONE, 0);
					this->asignarEstado(EST_QUIETO);
					return AF_STATE_CHANGE;
				} else {
					return AF_NONE;
				}
			} else {
				// enRango && !(state == accion)
				this->asignarEstado(accionAEstado[accion]);
				return AF_STATE_CHANGE;
			}
		}
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






/* void Unidad::mover(Escenario* scene) {
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
			this->asignarPos(&nuevaPos);
		} else {
			if(scene->verMapa()->posicionEstaVacia(&nuevaPos)){
				this->asignarPos(&nuevaPos);
			} else {
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
				nuevaPos = Posicion(aux.getX(), aux.getY());
				scene->moverEntidad(this, &nuevaPos);
			}
		}
 	} else {
		this->camino.pop_front();
		if (camino.size() == 0){
			this->setEstado(EST_QUIETO);
			this->asignarPos( new Posicion(act->getRoundX()+0.44,act->getRoundY()+0.44));
		}
 	}
} */