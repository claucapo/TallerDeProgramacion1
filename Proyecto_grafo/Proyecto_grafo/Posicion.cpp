#include "Posicion.h"


Posicion::Posicion(int x, int y)
{
	coord_x = x;
	coord_y = y;
}


Posicion::~Posicion(void)
{
}

int Posicion::devolverCoordX(){
	return coord_x;
}

int Posicion::devolverCoordY(){
	return coord_y;
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

