#include "Entidad.h"
#include "Posicion.h"
#include "Enumerados.h"
#include "ErrorLog.h"
#include <iostream>


// El constructor sobrecargado inicializa la entidad en una posición determinada
// por la referencia del parámetro
Entidad::Entidad(Posicion* p) {
	if (!p) {
		this->pos = nullptr;
		ErrorLog::getInstance()->escribirLog("Error al querer asignar Posicion a " + this->name + ": Posicion inexistente.", LOG_ERROR);
		}
	 else 
		this->pos = new Posicion(*p);
	this->state = EST_QUIETO;
	this->name = nombre_entidad_def;
	this->tamX = 1;
	this->tamY = 1;
	this->rangoVision = 4;

	this->owner = nullptr;
}

// El constructor por defecto inicializa la posición en nullptr
Entidad::Entidad(void) {
	this->pos = nullptr;
	this->sprites = nullptr;
	this->state = EST_QUIETO;
	this->name = nombre_entidad_def;
	this->tamX = 1;
	this->tamY = 1;
	this->rangoVision = 4;
	
	this->owner = nullptr;
}

Entidad::Entidad(string name, int tamX, int tamY, int vision) {
	this->pos = nullptr;
	this->sprites = nullptr;
	this->name = name;
	this->state = EST_QUIETO;
	this->tamX = (tamX > 0) ? tamX : 1;
	this->tamY = (tamY > 0) ? tamY : 1;
	this->rangoVision = vision;

	this->owner = nullptr;
}

// Llamo al destructor de todos los miembros de la clase (en caso de que alguno
// necesitara librar memoria)
Entidad::~Entidad(void) {
	if (this->pos)
		delete this->pos;
	if (this->sprites)
		delete this->sprites;
}

// Método para avanzar un frame
// Devuelve true si hay que quitar la entidad del escenario
bool Entidad::avanzarFrame(Escenario* scene) {
	this->sprites->siguienteFrame();
	return false;
}

void Entidad::asignarPos(Posicion* pos) {
	if(pos) {
		delete this->pos;
		this->pos = new Posicion(*pos);		
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
