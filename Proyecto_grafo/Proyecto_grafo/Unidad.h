#include "Posicion.h"
#include "Entidad.h"
#include "Enumerados.h"
#pragma once

/* La clase Unidad (derivada de Entidad)
representa cada uno de los elementos m�viles
(alias personajes) del juego. Esta clase es
una base abstracta de la cual derivaran las
unidades genuinas del juego.*/
class Unidad : public Entidad {
protected:
	Posicion* destino;
	float rapidez;
	Direcciones_t direccion;
	
public:
	// Constructor y destructor por defecto
	Unidad(void);
	~Unidad(void);
	
	Unidad(string name, int tamX, int tamY, int vision);

	Unidad(Posicion* p);

	// Devuelve el valor de la velocidad de
	// la Unidad.
	// NOTA: Debemos decidir una "unidad" para
	// medir la velocidad de las cosas.
	float verVelocidad(void) {return (this->rapidez * VEL_CONST);}

	Direcciones_t verDireccion(void) {return this->direccion;}
	Posicion* verDestino(void) {return this->destino;}

	// Si entend� bien a Juanma, con esto se
	// asigna una direcci�n respecto del sistema
	// coordenado del escenario para que la unidad
	// se mueva hacia all�.
	virtual void nuevoDestino(Posicion* pos);

	// Aumenta la velocidad de la Unidad
	// a partir de la aceleracion recibida.
	virtual void setVelocidad(float nuevaVelocidad);
	virtual void setEstado(Estados_t nuevoEstado);
	
	virtual bool avanzarFrame(Escenario* scene);

	Direcciones_t calcularDirecion(float velocidadX, float velocidadY);
public:
	void setDireccion(Direcciones_t);
};