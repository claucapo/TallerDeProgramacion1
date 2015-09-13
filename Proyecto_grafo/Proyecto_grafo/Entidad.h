#ifndef ENTIDAD_H
#define ENTIDAD_H

#include "Posicion.h"
#include "Spritesheet.h"
#include "Enumerados.h"

/* La clase Entidad representa cualquier
elemento que pueda ubicarse sobre una
Posicion. El Estado asociado a la Entidad
depende de lo que "est� haciendo" ella.
NOTA: Esta clase es una base, y de ella
deber�n heredar los "entes" verdaderos.*/
class Entidad {
protected:
	Posicion* pos;
	string name;
	Estados_t state;

	int tamX;
	int tamY;


public:
	// Constructor y destructor por defecto
	Entidad(void);
	~Entidad(void);

	// Constructor sobrecargado
	Entidad(Posicion* p);
	
	// Devuelve la Posicion sobre la cual
	// est� asociada la Entidad.
	// NOTA: Si la Entidad ocupa m�s de
	// una Posicion, devuelve la Posicion
	// en la que ir�a el pixel origen.
	Posicion* verPosicion(void) {return this->pos;}

	// Devuelve el estado
	Estados_t verEstado(void) {return this->state;}

	// Devuelve los dos tamanios
	int verTamX() {return this->tamX;}
	int verTamY() {return this->tamY;}

	// Avanza un frame modificando los valores de la entidad
	virtual void avanzarFrame(void);

	// NOTA: Sobrescribir los operadores
	// ==, < y >.
	// "Dos Entidad son iguales si ocupan
	// las mismas casillas y tienen los
	// mismos estados".

	// Asigna una Posicion de prepo
	void asignarPos(Posicion* pos);

	bool operator ==(const Entidad other);
};

#endif ENTIDAD_H