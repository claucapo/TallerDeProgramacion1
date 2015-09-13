#ifndef POSICION_H
#define POSICION_H


#pragma once

#include <cmath>


/* La clase Posicion representa la casilla (tile)
con coordenadas (x,y) dentro de la estructura del
mapa.
NOTA: Tal como está programa la clase por ahora,
si no se la inicializa con su constructor su
valor por defecto de (x,y) es (0,0).*/
class Posicion
{
private:
	// Atributos
	float coord_x, coord_y;
public:
	// Constructor y destructor por defecto
	Posicion();
	~Posicion(void);

	// Crea una Posicion con las coordenadas
	// xy recibidas.
	Posicion(float x, float y);
	
	// Devuelve las coordeadas en punto flotante
	float getX() {return this->coord_x;}
	float getY() {return this->coord_y;}

	// Devuelve los valores enteros de las coordenadas.
	// Como siempre son flotantes enteros, puedo utilizar la función floor.
	int getRoundX() const {return (int)std::floor(this->coord_x);}
	int getRoundY() const {return (int)std::floor(this->coord_y);}

	// Redefino el operador, dos posiciones son iguales si refieren
	// a la misma casilla, es decir, si tienen la misma parte entera
	bool operator == (const Posicion& other);
};

#endif //POSICION_H