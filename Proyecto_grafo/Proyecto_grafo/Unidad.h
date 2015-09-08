#include "Posicion.h"
#include "Entidad.h"
#include "Enumerados.h"
#pragma once

/* La clase Unidad (derivada de Entidad)
representa cada uno de los elementos móviles
(alias personajes) del juego. Esta clase es
una base abstracta de la cual derivaran las
unidades genuinas del juego.*/
class Unidad : public Entidad {
private:
	Posicion* destino;
	float rapidez;
	Direcciones_t direccion;
	
public:
	// Constructor y destructor por defecto
	Unidad(void);
	~Unidad(void);

	Unidad(Posicion* p);

	// Devuelve el valor de la velocidad de
	// la Unidad.
	// NOTA: Debemos decidir una "unidad" para
	// medir la velocidad de las cosas.
	float getVelocidad(void) {return this->rapidez;}

	// Si entendì bien a Juanma, con esto se
	// asigna una dirección respecto del sistema
	// coordenado del escenario para que la unidad
	// se mueva hacia allí.
	virtual void nuevoDestino(float x, float y);

	// Aumenta la velocidad de la Unidad
	// a partir de la aceleracion recibida.
	virtual void setVelocidad(float nuevaVelocidad);

	virtual void asignarSprite(string name);

	virtual void avanzarFrame(void);

private:
	Direcciones_t calcularDirecion(float velocidadX, float velocidadY);
};