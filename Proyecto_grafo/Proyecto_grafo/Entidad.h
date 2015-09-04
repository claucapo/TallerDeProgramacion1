#include "Posicion.h"
#include "EstadoEntidad.h"

#pragma once

/* La clase Entidad representa cualquier
elemento que pueda ubicarse sobre una
Posicion. El Estado asociado a la Entidad
depende de lo que "esté haciendo" ella.
NOTA: Esta clase es una base, y de ella
deberán heredar los "entes" verdaderos.*/
class Entidad
{
public:
	// Constructor y destructor por defecto
	Entidad(void);
	~Entidad(void);
	// Constructor sobrecargado

	Entidad(Posicion p);
	// Devuelve la Posicion sobre la cual
	// está asociada la Entidad.
	// NOTA: ¿Qué hacemos si la Entidad
	// ocupa más de una Posicion? ¿lista
	// de Posicion?
	virtual Posicion verPosicion(void);
	
	// Devuelve el EstadoEntidad asoaciado
	// actualmente a la Entidad.
	virtual EstadoEntidad verEstado(void);

	// NOTA: Sobrescribir los operadores
	// ==, < y >.
	// "Dos Entidad son iguales si ocupan
	// las mismas casillas y tienen los
	// mismos estados".
};

