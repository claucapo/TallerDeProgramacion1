#include "Posicion.h"
#include "Spritesheet.h"
#include "Enumerados.h"

#pragma once

/* La clase Entidad representa cualquier
elemento que pueda ubicarse sobre una
Posicion. El Estado asociado a la Entidad
depende de lo que "esté haciendo" ella.
NOTA: Esta clase es una base, y de ella
deberán heredar los "entes" verdaderos.*/
class Entidad {
protected:
	Posicion* pos;
	Spritesheet* sprites;
	string spriteBaseName;
	Estados_t state;


public:
	// Constructor y destructor por defecto
	Entidad(void);
	~Entidad(void);

	// Constructor sobrecargado
	Entidad(Posicion* p);
	
	// Devuelve la Posicion sobre la cual
	// está asociada la Entidad.
	// NOTA: Si la Entidad ocupa más de
	// una Posicion, devuelve la Posicion
	// en la que iría el pixel origen.
	Posicion* verPosicion(void) {return this->pos;}
	
	// Devuelve los sprites asociados a la entidad
	Spritesheet* verSprites(void) {return this->sprites;}

	// Devuelve el estado
	Estados_t verEstado(void) {return this->state;}

	// Avanza un frame modificando los valores de la entidad
	virtual void avanzarFrame(void);

	// Metodos para setear los sprites.
	void asignarSprite(string name);

	// NOTA: Sobrescribir los operadores
	// ==, < y >.
	// "Dos Entidad son iguales si ocupan
	// las mismas casillas y tienen los
	// mismos estados".

	// Asigna una Posicion de prepo
	void asignarPos(Posicion* pos);

	bool operator ==(const Entidad other);
};