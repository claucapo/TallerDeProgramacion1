#include "Posicion.h"
#include "Entidad.h"
#pragma once

/* La clase Unidad (derivada de Entidad)
representa cada uno de los elementos m�viles
(alias personajes) del juego. Esta clase es
una base abstracta de la cual derivaran las
unidades genuinas del juego.*/
class Unidad : public Entidad
{
public:
	// Constructor y destructor por defecto
	Unidad(void);
	~Unidad(void);

	// Devuelve el valor de la velocidad de
	// la Unidad.
	// NOTA: Debemos decidir una unidad para
	// medir la velocidad de las cosas.
	double verVelocidad(void);

	// Si entend� bien a Juanma, con esto se
	// asigna una direcci�n respecto del sistema
	// coordenado del escenario para que la unidad
	// se mueva hacia all�.
	virtual void cambiarDireccion(float x, float y);

	// Aumenta la velocidad de la Unidad
	// a partir de la aceleracion recibida.
	virtual void acelerar(double aceleracion);

	// Recalcula la nueva Posicion de la
	// Unidad seg�n sus coordenadas actuales
	// y su velocidad.
	virtual Posicion recalcularPosicion(void);
};

