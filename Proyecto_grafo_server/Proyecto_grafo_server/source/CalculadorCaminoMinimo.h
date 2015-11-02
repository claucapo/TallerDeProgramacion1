#include "Nodo.h"
#include <string>
#include "Posicion.h"
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
	list<Posicion*> CalculadorCaminoMinimo::calcularCaminoMinimo( int xAct, int yAct, int xDest, int yDest, int** mapDeOcupaciones);
};
