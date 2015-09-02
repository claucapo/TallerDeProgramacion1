#include "Grafo.h"
#include "Posicion.h"
#pragma once
class Escenario
{
private:
	// El grafo siguiente es el mapa
	Grafo mapa;
	// Métodos internos
	void generarMapaVacio(int casillas_x, int casillas_y);
public:
	Escenario(int casillas_x, int casillas_y);
	~Escenario(void);
	Grafo verMapa(){return mapa;} ;

};

