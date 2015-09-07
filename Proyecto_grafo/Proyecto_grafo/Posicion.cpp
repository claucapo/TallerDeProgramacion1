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


Posicion::~Posicion(void)
{
}

int Posicion::generarCodigo(){
	return coord_x * 1000 + coord_y;
}

bool Posicion::operator<(const Posicion other){
	return ((coord_x < other.coord_x)&&(coord_y < other.coord_y));
}

bool Posicion::operator>(const Posicion other){
	return ((coord_x > other.coord_x)&&(coord_y > other.coord_y));
}

bool Posicion::operator==(const Posicion other){
	return ((coord_x == other.coord_x)&&(coord_y == other.coord_y));
}

