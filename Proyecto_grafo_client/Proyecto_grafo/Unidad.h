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
protected:
	Posicion* destino;
	float rapidez;
	Direcciones_t direccion;

	Unidad(void);
	Unidad(Posicion* p);

public:
	// Constructor y destructor por defecto

	~Unidad(void);
	
	Unidad(unsigned int id, string name, int tamX, int tamY, int vision, int velocidad);
	
	// Devuelve el valor de la velocidad de
	// la Unidad.
	float verVelocidad(void) {return (this->rapidez * VEL_CONST);}

	Direcciones_t verDireccion(void) {return this->direccion;}
	Posicion* verDestino(void) {return this->destino;}

	// Si entendì bien a Juanma, con esto se
	// asigna una dirección respecto del sistema
	// coordenado del escenario para que la unidad
	// se mueva hacia allí.
	virtual void nuevoDestino(Posicion* pos);

	// Aumenta la velocidad de la Unidad
	// a partir de la aceleracion recibida.
	virtual void setVelocidad(float nuevaVelocidad);
	virtual void setEstado(Estados_t nuevoEstado);
	
	virtual af_result_t avanzarFrame(Escenario* scene);

	Direcciones_t calcularDirecion(float velocidadX, float velocidadY);
public:
	void setDireccion(Direcciones_t);
};