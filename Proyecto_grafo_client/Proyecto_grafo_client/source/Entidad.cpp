#include "Entidad.h"
#include "Posicion.h"
#include "Enumerados.h"
#include "Jugador.h"
#include "ErrorLog.h"
#include <iostream>


Entidad::Entidad(unsigned int id, string name, tipoEntidad_t pType) {
	this->id = id;
	this->typeID = pType.typeID;
	this->name = name;
	this->tipo = pType.tipo;
	this->pos = nullptr;
	this->owner = nullptr;
	this->state = EST_QUIETO;
	this->luck = pType.luck;

	this->accion = ACT_NONE;
	this->targetID = this->id;

	this->tamX = (pType.tamX > 0) ? pType.tamX : 1;
	this->tamY = (pType.tamY > 0) ? pType.tamY : 1;
	this->rangoVision = pType.rangoV;

	this->vidaAct = pType.vidaMax;
	this->vidaMax = pType.vidaMax;
	this->ataque = pType.ataque;
	this->defensa = pType.defensa;

	this->sprites = NULL;

	this->costo = pType.costo;
	this->trainRate = pType.trainRate;

	this->cooldownMax = pType.cooldown;
	this->cooldownAct = 0;
	for (int i = 0; i < CANT_ACCIONES; i++) {
		this->habilidades[i] = pType.habilidades[i];	
	}


	this->casillasVistas = list<Posicion>();
	Posicion pos = Posicion(-this->rangoVision, 0);
	int diagonales = 1 + this->tamX + this->tamY - 2 + 2*(this->rangoVision);
	int diagX = -this->rangoVision;
	int diagY = 0;
	int diagSize = this->rangoVision + 1;
	int actX, actY;
	bool nextDown = false;
	for (int i = 0; i < diagonales; i++) {
		for (int j = 0; j < diagSize; j++) {
			actX = diagX + j;
			actY = diagY - j;
			pos = Posicion(actX, actY);
			this->casillasVistas.push_back(pos);
		}
		if (diagY < (this->tamY - 1)) {
			diagY++;
			diagSize++;
		} else {
			if (nextDown) {
				diagY++;
				diagSize++;
				nextDown = false;
			} else {
				diagX++;
				diagSize--;
				if (!(diagY == this->tamY + this->rangoVision - 1))
					nextDown = true;
			}
		}
	}

}

// Llamo al destructor de todos los miembros de la clase (en caso de que alguno
// necesitara librar memoria)
Entidad::~Entidad(void) {
	if (this->pos != NULL)
		delete this->pos;
	if (this->sprites != NULL)
		delete this->sprites;
}

// Método para avanzar un frame
// Devuelve true si hay que quitar la entidad del escenario
af_result_t Entidad::avanzarFrame(Escenario* scene) {
	this->sprites->siguienteFrame();
	return AF_NONE;
}

void Entidad::asignarPos(Posicion* pos) {
	if(pos) {
		delete this->pos;
		this->pos = new Posicion(*pos);
		cout<<"imposible!"<<endl;
	}
	else
		ErrorLog::getInstance()->escribirLog("Error al querer asignar Posicion a " + this->name + ": Posicion inexistente.", LOG_ERROR);
}

void Entidad::asignarSprite(Spritesheet* sp){
	this->sprites = sp;
}

void Entidad::asignarJugador(Jugador* player) {
	if (!player)
		ErrorLog::getInstance()->escribirLog("Error al querer asignar un jugador a " + this->name + ": Jugador inexistente.", LOG_ERROR);
	else
		this->owner = player;
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

Spritesheet* Entidad::verSpritesheet(void) {
	return sprites;
}

void Entidad::settearEstado(Estados_t state) {
	this->state = state;
	
}