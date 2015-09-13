#include "Posicion.h"

Posicion::Posicion()
{
	coord_x = 0;
	coord_y = 0;
}

Posicion::Posicion(float x, float y) {
	coord_x = x;
	coord_y = y;
}

Posicion::~Posicion(void){
}

bool Posicion::operator==(const Posicion& other){
	return ((getRoundX() == other.getRoundX()) && (getRoundX() == other.getRoundY()));
}

