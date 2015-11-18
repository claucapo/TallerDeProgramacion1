#include "Posicion.h"
#include "Entidad.h"
#include "Enumerados.h"
#include <list>
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

	list<Posicion*> camino;

	int collectRate, buildRate;

private:
	void mover(Escenario* scene);
	bool realizarAccion(Accion_t acc, Entidad* target, Escenario* scene);

	
	bool resolverAtaque(Entidad* target, Escenario* scene);
	bool resolverRecoleccion(Entidad* target, Escenario* scene);
	bool resolverConstruccion(Entidad* target, Escenario* scene);

	int calcularDamage(Entidad* target);
	
public:
	// Constructor y destructor por defecto

	~Unidad(void);
	
	Unidad(unsigned int id, string name, tipoEntidad_t pType);
	
	// Devuelve el valor de la velocidad de
	// la Unidad.
	float verVelocidad(void) {return (this->rapidez * VEL_CONST);}

	Direcciones_t verDireccion(void) {return this->direccion;}
	Posicion* verDestino(void) {return this->destino;}
	Posicion* verPosicion(void) {return this->pos;}

	// Si entendì bien a Juanma, con esto se
	// asigna una dirección respecto del sistema
	// coordenado del escenario para que la unidad
	// se mueva hacia allí.
	virtual void nuevoDestino(Posicion* pos);

	// Aumenta la velocidad de la Unidad
	// a partir de la aceleracion recibida.
	virtual void setVelocidad(float nuevaVelocidad);
	virtual void marcarCamino(list<Posicion*> camino);

	virtual bool objetivoEnRango(Entidad* target, Escenario* scene);

	virtual bool puedeRealizarAccion(Accion_t acc);
	virtual void asignarAccion(Accion_t acc, unsigned int targetID);
	
	virtual af_result_t avanzarFrame(Escenario* scene);

	Direcciones_t calcularDirecion(float velocidadX, float velocidadY);

	void setDireccion(Direcciones_t);
};