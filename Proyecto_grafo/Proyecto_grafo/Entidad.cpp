#include "Entidad.h"
#include "Posicion.h"
#include <iostream>

// El constructor sobrecargado inicializa la entidad en una posici�n determinada
// por la referencia del par�metro
Entidad::Entidad(Posicion* p) {
	if (!p) 
		this->pos = nullptr;
	 else 
		this->pos = new Posicion(*p);
	this->state = EST_QUIETO;
	this->tamX = 1;
	this->tamY = 1;
}

// El constructor por defecto inicializa la posici�n en nullptr
Entidad::Entidad(void) {
	this->pos = nullptr;
	this->state = EST_QUIETO;
	this->tamX = 1;
	this->tamY = 1;
}

// Llamo al destructor de todos los miembros de la clase (en caso de que alguno
// necesitara librar memoria)
Entidad::~Entidad(void) {
	if (this->pos)
		delete this->pos;
	if (this->sprites)
		delete this->sprites;
}

// M�todo para avanzar un frame
void Entidad::avanzarFrame(void) {
	this->sprites->siguienteFrame();
}

void Entidad::asignarPos(Posicion* pos) {
	if(pos) {
		delete this->pos;
		this->pos = new Posicion(*pos);		
	}
}

void Entidad::asignarSprite(Spritesheet* sp){
	this->sprites = sp;
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
