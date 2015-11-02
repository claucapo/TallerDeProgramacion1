#include "Posicion.h"
#include <string>
#include <sstream>
using namespace std;

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


string Posicion::toStr() {
	std::stringstream s;
	s << "(" << this->coord_x << "," << this->coord_y << ")";
	return s.str();
}

string Posicion::toStrRound() {
	std::stringstream s;
	s << "(" << this->getRoundX() << "," << this->getRoundY() << ")";
	return s.str();
}


bool Posicion::operator==(const Posicion& other){
	return ((getRoundX() == other.getRoundX()) && (getRoundX() == other.getRoundY()));
}

bool Posicion::operator<(const Posicion& other){
	if (*this == other)
		return (this->getRoundY() < other.getRoundY());
	int dist1 = this->getRoundX() + this->getRoundY();
	int dist2 = other.getRoundX() + other.getRoundY();
	if (dist1 < dist2)
		return true;
	else
		return false;
}

bool Posicion::operator>(const Posicion& other){
	if (*this == other)
		return (this->getRoundY() > other.getRoundY());
	int dist1 = this->getRoundX() + this->getRoundY();
	int dist2 = other.getRoundX() + other.getRoundY();
	if (dist1 > dist2)
		return true;
	else
		return false;
}

