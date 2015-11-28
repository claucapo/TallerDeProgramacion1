#include "Nodo.h"
#include <string>
#include "Posicion.h"
#include "Enumerados.h"
#include <list>

#pragma once

class CalculadorCaminoMinimo
{

private:
	int filas;
	int columnas;
	int** listaCerrada;
	int** listaAbierta;
	int** listaDirecciones;

public:
	CalculadorCaminoMinimo(void);
	CalculadorCaminoMinimo(int filas,int columnas);
	~CalculadorCaminoMinimo(void);
	list<Posicion*> CalculadorCaminoMinimo::calcularCaminoMinimo( int xAct, int yAct, int xDest, int yDest, int** mapDeOcupaciones, terrain_type_t** mapDeTerreno, terrain_type_t validTerrain = TERRAIN_GRASS);
};
