#include "Nodo.h"
#include <string>

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
	string CalculadorCaminoMinimo::calcularCaminoMinimo( int xAct, int yAct, int xDest, int yDest, int** mapDeOcupaciones);
};
